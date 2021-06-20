#include <rocketsd/module/cute/cute_module.hh>

#include <sstream>

#include <QTimer>

#include <shared/interfaces/radio/radio_packet.h>

#include <log/log.hh>
#include <util/json.hh>
#include <util/time.hh>
#include <util/switch.hh>
#include <protocol/name.hh>
#include <rocketsd/module/cute/qt_socket_adapter.hh>

namespace rocketsd::modules::cute {

struct CuteModule::Priv {
    std::string path;
    std::string host;
    unsigned port;

    std::unique_ptr<QtSocketAdapter> socket;
};

CuteModule::CuteModule(QObject* parent, rocketsd::protocol::ProtocolSP protocol)
    : Module(parent, protocol)
    , _d(new CuteModule::Priv)
{
}

CuteModule::~CuteModule()
{
    if (_d->socket) {
        _d->socket->disconnect();
    }
}

bool CuteModule::init(json& config)
{
    if (!util::json::validate("CuteModule", config,
            util::json::optionnal(_d->path, "path", ""),
            util::json::optionnal(_d->host, "host", ""),
            util::json::optionnal(_d->port, "port", 0u)
        )) {
        return false;
    }

    if (_d->path == "" && (_d->host == "" || _d->port == 0)) {
        return false;
    }

    connect();

    logging::info("CuteModule") << "Successfully init'd CuteStation client" << logging::endl;
    return true;
}

void CuteModule::onPacket(radio_packet_t packet)
{
    rocketsd::protocol::Node* node;
    rocketsd::protocol::Message* message;

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        logging::warn("CuteModule") << "Could not find Node with id=" << packet.node << ": ignoring" << logging::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        logging::warn("CuteModule") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << logging::endl;
        return;
    }

    ::cute::proto::Data* data = new ::cute::proto::Data();
    ::cute::proto::Measurement& measurement = *(data->add_measurements());
    measurement.set_source(std::string("anirniq.") + std::string(node->name()) + "." + std::string(message->name()));
    measurement.set_timestamp(util::time::now<std::milli>());

    util::switcher::string(std::string(message->type()), {
        {"int", [&packet, &measurement]() { measurement.set_int_(packet.payload.INT); }},
        {"uint", [&packet, &measurement]() { measurement.set_int_(packet.payload.UINT); }},
        {"float", [&packet, &measurement]() { measurement.set_float_(packet.payload.FLOAT); }},
    }, [&message]() {logging::err("CuteModule") << "Unhandled payload type '" << message->type() << "'" << logging::endl; });

    std::shared_ptr<::cute::proto::Packet> cutepacket = std::make_shared<::cute::proto::Packet>();
    cutepacket->set_allocated_data(data);
    dispatch(cutepacket);
}

void CuteModule::dispatch(std::shared_ptr<::cute::proto::Packet> packet)
{
    if (_d->socket->isConnected()) {
        void* buffer[512];
        packet->SerializeToArray(buffer, 512);

        uint64_t size = packet->ByteSizeLong();
        _d->socket->device()->write((char*)&size, sizeof(size));
        _d->socket->device()->write((char*)buffer, packet->ByteSizeLong());
    }
}

void CuteModule::onSocketData()
{
    if (_d->socket->isConnected()) {
        uint64_t size;

        _d->socket->device()->read((char*)&size, sizeof(size));
        uint8_t* buffer = new uint8_t[size];

        _d->socket->device()->read((char*)buffer, size);
        std::shared_ptr<::cute::proto::Packet> cutepacket = std::make_shared<::cute::proto::Packet>();
        cutepacket->ParseFromArray(buffer, size);
        delete[] buffer;

        if (!cutepacket->has_data()) {
            logging::warn("CuteModule") << "Received unexpected message from CuteStation" << logging::endl;
            return;
        } else {
            logging::debug("CuteModule") << "Received data from CuteStation" << logging::endl;
        }

        for (const ::cute::proto::Measurement& measurement : cutepacket->data().measurements()) {
            rocketsd::protocol::Node* node = nullptr;
            rocketsd::protocol::Message* message = nullptr;

            if (rocketsd::protocol::from_cute_name(protocol_.get(), measurement.source(), &node, &message)) {
                radio_packet_t packet;
                memset((void*)&packet, 0, sizeof(radio_packet_t));

                packet.node = node->id();
                packet.message_id = message->id();
                
                switch (measurement.value_case()) {
                    case ::cute::proto::Measurement::kBool:
                        packet.payload.INT = 1;
                        break;
                    case ::cute::proto::Measurement::kInt:
                        packet.payload.INT = measurement.int_();
                        break;
                    case ::cute::proto::Measurement::kFloat:
                        packet.payload.FLOAT = measurement.float_();
                        break;
                    default:
                        logging::info("CuteModule") <<  "Received unhandled message type from CuteStation" << logging::endl;
                        return;
                }
        
                packet.checksum = radio_compute_crc(&packet);
                emit packetReady(packet);
            }
        }
    }
}

void CuteModule::connect()
{
    if (!_d->socket) {
        QtSocketAdapter* adapter = nullptr;

        if (_d->path != "") {
            adapter = new QtSocketAdapter(this, new QLocalSocket, _d->path);
        } else if (_d->host != "" && _d->port != 0) {
            adapter = new QtSocketAdapter(this, new QTcpSocket, _d->host, _d->port);
        }

        assert(adapter);
        if (adapter) {
            QObject::connect(adapter, &QtSocketAdapter::connected, this, &CuteModule::onConnected);
            QObject::connect(adapter, &QtSocketAdapter::disconnected, this, &CuteModule::onDisconnected);
            QObject::connect(adapter, &QtSocketAdapter::errorOccured, this, &CuteModule::onError);
            QObject::connect(adapter, &QtSocketAdapter::readyRead, this, &CuteModule::onSocketData);
            _d->socket.reset(adapter);
        }
    }
}

void CuteModule::onConnected()
{
    logging::info("CuteModule")
        << "Connected to CuteStation" 
        << (_d->path != "" ? logging::tag{"path", _d->path} : logging::tag{"host", _d->host + ":" + std::to_string(_d->port) })
        << logging::endl;

    ::cute::proto::Handshake* handshake = new ::cute::proto::Handshake();
    handshake->set_name("rocketsd");

    for (std::pair<int, rocketsd::protocol::Node*> node: *protocol_) {
        for (std::pair<int, rocketsd::protocol::Message*> message: *(node.second)) {
            if (message.second->command()) {
                auto* command = handshake->add_commands();
                command->set_name(rocketsd::protocol::to_cute_name(protocol_.get(), node.second, message.second));
                command->set_type(::cute::proto::Handshake_Command_Type_BOOL);
            }
        }
    }

    std::shared_ptr<::cute::proto::Packet> cutepacket = std::make_shared<::cute::proto::Packet>();
    cutepacket->set_allocated_handshake(handshake);
    dispatch(cutepacket);
}

void CuteModule::onError(QAbstractSocket::SocketError /*error*/)
{
    _d->socket.reset();
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, this, &CuteModule::connect);
    QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(1000);
}

void CuteModule::onDisconnected()
{
    _d->socket.reset();
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, this, &CuteModule::connect);
    QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(1000);
}

} // namespace