#include "cute_module.hh"

#include <sstream>

#include <QTimer>

#include <log/log.hh>
#include <util/json.hh>
#include <util/time.hh>
#include <util/switch.hh>
#include <protocol/name.hh>

namespace modules {

CuteModule::CuteModule(QObject* parent, rocketsd::protocol::ProtocolSP protocol)
    : Module(parent, protocol)
    , socket_(new QLocalSocket(this))
{
}

CuteModule::~CuteModule()
{
    socket_->disconnect();
}

bool CuteModule::init(json& config)
{
    if (!util::json::validate("CuteModule", config, util::json::required(socket_path_, "socket"))) {
        return false;
    }

    QObject::connect(socket_, &QLocalSocket::connected, this, &CuteModule::onConnected);
    QObject::connect(socket_, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &CuteModule::onError);
    QObject::connect(socket_, &QLocalSocket::disconnected, this, &CuteModule::connect);
    QObject::connect(socket_, &QLocalSocket::readyRead, this, &CuteModule::onSocketData);
    connect();

    Log::info("CuteModule") << "Successfully init'd CuteStation client" << std::endl;
    return true;
}

void CuteModule::onPacket(radio_packet_t packet)
{
    rocketsd::protocol::Node* node;
    rocketsd::protocol::Message* message;

    if (socket_->state() != QLocalSocket::ConnectedState) {
        return;
    }

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        Log::warn("CuteModule") << "Could not find Node with id=" << packet.node << ": ignoring" << std::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        Log::warn("CuteModule") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << std::endl;
        return;
    }

    cute::proto::Data* data = new cute::proto::Data();
    cute::proto::Measurement& measurement = *(data->add_measurements());
    measurement.set_source(std::string("anirniq.") + std::string(node->name()) + "." + std::string(message->name()));
    measurement.set_timestamp(util::now<std::milli>());

    util::switcher::string(std::string(message->type()), {
        {"int", [&packet, &measurement]() { measurement.set_int_(packet.payload.INT); }},
        {"uint", [&packet, &measurement]() { measurement.set_int_(packet.payload.UINT); }},
        {"float", [&packet, &measurement]() { measurement.set_float_(packet.payload.FLOAT); }},
    }, [&message]() {Log::err("CuteModule") << "Unhandled payload type '" << message->type() << "'" << std::endl; });

    std::shared_ptr<cute::proto::Packet> cutepacket = std::make_shared<cute::proto::Packet>();
    cutepacket->set_allocated_data(data);
    dispatch(cutepacket);
}

void CuteModule::dispatch(std::shared_ptr<cute::proto::Packet> packet)
{
    // TODO: Once protocol is better defined, figure out max possible size of a
    // packet and make sure it fits in this buffer. As of 2020-02-03 packet size
    // is 38 bytes.
    void* buffer[512];
    packet->SerializeToArray(buffer, 512);

    uint64_t size = packet->ByteSizeLong();
    socket_->write((char*)&size, sizeof(size));
    socket_->write((char*)buffer, packet->ByteSizeLong());
}

void CuteModule::onSocketData()
{
    uint64_t size;

    socket_->read((char*)&size, sizeof(size));
    uint8_t* buffer = new uint8_t[size];
    socket_->read((char*)buffer, size);
    std::shared_ptr<cute::proto::Packet> cutepacket = std::make_shared<cute::proto::Packet>();
    cutepacket->ParseFromArray(buffer, size);
    delete[] buffer;

    if (!cutepacket->has_data()) {
        Log::warn("CuteModule") << "Received unexpected message from CuteStation" << std::endl;
        return;
    } else {
        Log::debug("CuteModule") << "Received data from CuteStation" << std::endl;
    }

    for (const cute::proto::Measurement& measurement : cutepacket->data().measurements()) {
        rocketsd::protocol::Node* node = nullptr;
        rocketsd::protocol::Message* message = nullptr;

        if (rocketsd::protocol::from_cute_name(protocol_.get(), measurement.source(), &node, &message)) {
            radio_packet_t packet;
            memset((void*)&packet, 0, sizeof(radio_packet_t));

            packet.node = node->id();
            packet.message_id = message->id();
            
            switch (measurement.value_case()) {
                case cute::proto::Measurement::kBool:
                    packet.payload.INT = 1;
                    break;
                case cute::proto::Measurement::kInt:
                    packet.payload.INT = measurement.int_();
                    break;
                case cute::proto::Measurement::kFloat:
                    packet.payload.FLOAT = measurement.float_();
                    break;
                default:
                    Log::info("CuteModule", "Received unhandled message type from CuteStation");
                    return;
            }
    
            packet.checksum = radio_compute_crc(&packet);
            emit packetReady(packet);
        }
    }
}

void CuteModule::connect()
{
    socket_->connectToServer(socket_path_.c_str());
}

void CuteModule::onConnected()
{
    Log::info("CuteModule") << "Connected to CuteStation on " << socket_path_ << std::endl;
    cute::proto::Handshake* handshake = new cute::proto::Handshake();
    handshake->set_name("rocketsd");

    for (std::pair<int, rocketsd::protocol::Node*> node: *protocol_) {
        for (std::pair<int, rocketsd::protocol::Message*> message: *(node.second)) {
            if (message.second->command()) {
                auto* command = handshake->add_commands();
                command->set_name(rocketsd::protocol::to_cute_name(protocol_.get(), node.second, message.second));
                command->set_type(cute::proto::Handshake_Command_Type_BOOL);
            }
        }
    }

    std::shared_ptr<cute::proto::Packet> cutepacket = std::make_shared<cute::proto::Packet>();
    cutepacket->set_allocated_handshake(handshake);
    dispatch(cutepacket);
}

void CuteModule::onError(QLocalSocket::LocalSocketError error)
{
    if (socket_->state() == QLocalSocket::ConnectedState) {
        return;
    }

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, this, &CuteModule::connect);
    QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(1000);
}

} // namespace