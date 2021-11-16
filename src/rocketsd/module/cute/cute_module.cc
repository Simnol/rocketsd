
#include <sstream>

#include <QTimer>

#include <log/log.hh>
#include <util/json.hh>
#include <util/time.hh>
#include <util/switch.hh>

#include "cute_module.hh"
#include "qt_socket_adapter.hh"

namespace rocketsd::modules::cute {

namespace {
    struct CommandConfiguration {
        std::string command;
        ::cute::proto::Handshake::Command::Type type;
    };
}


struct CuteModule::Priv {
    std::string path;
    std::string host;
    unsigned port;

    std::vector<CommandConfiguration> commands;

    QtSocketAdapter* socket = nullptr;
};

CuteModule::CuteModule(QObject* parent)
    : Module(parent)
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
    if (!Module::init(config)) {
        return false;
    }

    if (!util::json::validate("CuteModule", config,
            util::json::optional(_d->path, "path", ""),
            util::json::optional(_d->host, "host", ""),
            util::json::optional(_d->port, "port", 0u)
        )) {
        return false;
    }

    if (config.count("commands") && config["commands"].is_array()) {
        for (const auto subconfig: config["commands"]) {
            if (!subconfig.is_object()) {
                logging::err("CuteModule") << "Invalid JSON type for command configuration: ignoring" << logging::endl;
                continue;
            }

            std::string name, type;
            if (!util::json::validate("CuteModule", subconfig,
                util::json::required(name, "name"),
                util::json::required(type, "type")
            )) {
                continue;
            }

            util::switcher::string(type, {
                {"number", [&]() { _d -> commands.emplace_back(name, ::cute::proto::Handshake_Command_Type_NUMBER); }},
                {"state", [&]() { _d -> commands.emplace_back(name, ::cute::proto::Handshake_Command_Type_STATE); }},
                {"string", [&]() { _d -> commands.emplace_back(name, ::cute::proto::Handshake_Command_Type_STRING); }},
                {"bool", [&]() { _d -> commands.emplace_back(name, ::cute::proto::Handshake_Command_Type_BOOL); }}
            }, [&]() {
                logging::err("CuteModule") << "Invalid command type for command configuration: ignoring" << logging::endl;
            });
        }
    }

    if (_d->path == "" && (_d->host == "" || _d->port == 0)) {
        return false;
    }

    connect();

    logging::info("CuteModule") << "Successfully init'd CuteStation client" << logging::tag{"id", id()} << logging::endl;
    return true;
}

void CuteModule::onMessage(Message message)
{
    if (message.measurement.timestamp() == 0) {
        message.measurement.set_timestamp(util::time::now<std::milli>());
    }

    std::shared_ptr<::cute::proto::Packet> cutepacket = std::make_shared<::cute::proto::Packet>();
    cutepacket->mutable_data()->add_measurements()->Swap(&message.measurement);
    dispatch(cutepacket);
}

void CuteModule::dispatch(std::shared_ptr<::cute::proto::Packet> packet)
{
    if (_d->socket && _d->socket->isConnected()) {
        void* buffer[512];
        packet->SerializeToArray(buffer, 512);

        uint64_t size = packet->ByteSizeLong();
        _d->socket->device()->write((char*)&size, sizeof(size));
        _d->socket->device()->write((char*)buffer, packet->ByteSizeLong());
    }
}

void CuteModule::onSocketData()
{
    if (_d->socket && _d->socket->isConnected()) {
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
            emit messageReady({this, measurement});
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
            _d->socket = adapter;
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

    for (const auto& command: _d -> commands) {
        auto cmd = handshake->add_commands();
        cmd -> set_name(command.command);
        cmd -> set_type(command.type);
    }

    std::shared_ptr<::cute::proto::Packet> cutepacket = std::make_shared<::cute::proto::Packet>();
    cutepacket->set_allocated_handshake(handshake);
    dispatch(cutepacket);
}

void CuteModule::onError(QAbstractSocket::SocketError error)
{
    if (_d->socket && error != QAbstractSocket::ConnectionRefusedError) {
        logging::err("CuteModule") << _d->socket->device()->errorString().toStdString() << logging::endl;
    }

    onDisconnected();
}

void CuteModule::onDisconnected()
{
    _d->socket->deleteLater();
    _d->socket = nullptr;

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, this, &CuteModule::connect);
    QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(1000);
}

} // namespace