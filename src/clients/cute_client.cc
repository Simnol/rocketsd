#include "clients/cute_client.h"

#include <QTimer>

#include "log.h"
#include "util.h"

namespace clients {

CuteClient::CuteClient(QObject* parent, ProtocolSP protocol)
    : Client(parent, protocol)
    , socket_(new QLocalSocket(this))
{
}

CuteClient::~CuteClient()
{
}

bool CuteClient::init(json& config)
{
    if (!(config.count("socket") && config["socket"].is_string())) {
        Log::warn("CuteClient") << "Missing or invalid mandatory configuration 'socket'" << std::endl;
        return false;
    }
    socket_path_ = config["socket"].get<std::string>();

    QObject::connect(socket_, &QLocalSocket::connected, this, &CuteClient::onConnected);
    QObject::connect(socket_, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &CuteClient::onError);
    QObject::connect(socket_, &QLocalSocket::disconnected, this, &CuteClient::connect);
    connect();

    Log::info("CuteClient") << "Successfully init'd CuteStation client" << std::endl;
    return true;
}

void CuteClient::handle(radio_packet_t packet)
{
    Node* node;
    Message* message;

    if (socket_->state() != QLocalSocket::ConnectedState) {
        return;
    }

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        Log::err("CuteClient") << "Could not find Node with id=" << packet.node << ": ignoring" << std::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        Log::err("CuteClient") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << std::endl;
        return;
    }

    PacketSP cutepacket = std::make_shared<Packet>();
    cutepacket->set_source(std::string("anirniq.") + std::string(node->name()) + "." + std::string(message->name()));
    cutepacket->set_value(packet.payload.FLOAT);
    cutepacket->set_timestamp(now());

    dispatch(cutepacket);
}

void CuteClient::dispatch(PacketSP packet)
{
    // TODO: Once protocol is better defined, figure out max possible size of a
    // packet and make sure it fits in this buffer. As of 2020-02-03 packet size
    // is 38 bytes.
    void* buffer[512];
    packet->SerializeToArray(buffer, 512);
    socket_->write((char*)buffer, packet->ByteSizeLong());
}

void CuteClient::connect()
{
    socket_->connectToServer(socket_path_.c_str());
}

void CuteClient::onConnected()
{
    Log::info("CuteClient") << "Connected to CuteStation on " << socket_path_ << std::endl;
}

void CuteClient::onError(QLocalSocket::LocalSocketError error)
{
    if (socket_->state() == QLocalSocket::ConnectedState) {
        return;
    }

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, this, &CuteClient::connect);
    QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(1000);

    Log::warn("CuteClient") << "Could not connect to CuteStation, trying again in 1 sec" << std::endl;
}

} // namespace