#include "modules/cute_module.h"

#include <sstream>

#include <QTimer>

#include "log.h"
#include "util.h"

namespace modules {

namespace {
template <class T, class U>
T* find(U* container, std::string& name)
{
    auto iter = std::find_if(container->begin(), container->end(), [name](auto item) { return item.second->name() == name; });

    if (iter != container->end()) {
        return iter->second;
    }
    return nullptr;
}

/** nameToNodeMessage
 * Finds Node & Message object within a given Protocol object that match the
 * provided CuteStation object name (e.g.: "anirniq.remote_control.eject")
 */
bool nameToNodeMessage(std::string name, const ProtocolSP protocol, Node** ndest, Message** mdest)
{
    std::stringstream ss(name);
    std::string temp;

    if (!std::getline(ss, temp, '.')) {
        // return error("unknown message id (" + name + ")");
        return false;
    }

    if (temp == "anirniq") {
        std::getline(ss, temp, '.');
    }

    *ndest = find<Node, Protocol>(protocol.get(), temp);
    if (!ndest) {
        // return error("no such node (" + temp + ")");
        return false;
    }

    temp = name.substr(ss.tellg());
    *mdest = find<Message, Node>(*ndest, temp);
    if (!mdest) {
        // return error("no such message (" + temp + ")");
        return false;
    }

    return true;
}

std::string messageToCuteName(const ProtocolSP protocol, const Node* node, const Message* message)
{
    std::stringstream ss;
    ss << "anirniq." << node->name() << "." << message->name();
    return ss.str();
}

}

CuteModule::CuteModule(QObject* parent, ProtocolSP protocol)
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
    if (!has<std::string>(config, "socket")) {
        Log::err("CuteModule") << "Missing or invalid mandatory configuration 'socket'" << std::endl;
        return false;
    }
    socket_path_ = config["socket"].get<std::string>();

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
    Node* node;
    Message* message;

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
    measurement.set_timestamp(now());

    if (message->type() == "float") {
        measurement.set_float_(packet.payload.FLOAT);
    } else if (message->type() == "int") {
        measurement.set_int_(packet.payload.INT);
    } else if (message->type() == "uint") {
        measurement.set_int_(packet.payload.UINT);
    } else {
        Log::err("CuteModule") << "Unhandled payload type '" << message->type() << "'" << std::endl;
    }

    cute::proto::PacketSP cutepacket = std::make_shared<cute::proto::Packet>();
    cutepacket->set_allocated_data(data);
    dispatch(cutepacket);
}

void CuteModule::dispatch(cute::proto::PacketSP packet)
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
    cute::proto::PacketSP cutepacket = std::make_shared<cute::proto::Packet>();
    cutepacket->ParseFromArray(buffer, size);
    delete[] buffer;

    if (!cutepacket->has_data()) {
        Log::warn("CuteModule") << "Received unexpected message from CuteStation" << std::endl;
        return;
    } else {
        Log::debug("CuteModule") << "Received data from CuteStation" << std::endl;
    }

    for (const cute::proto::Measurement& measurement : cutepacket->data().measurements()) {
        Node* node = nullptr;
        Message* message = nullptr;

        if (nameToNodeMessage(measurement.source(), protocol_, &node, &message)) {
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

    for (std::pair<int, Node*> node: *protocol_) {
        for (std::pair<int, Message*> message: *(node.second)) {
            if (message.second->command()) {
                auto* command = handshake->add_commands();
                command->set_name(messageToCuteName(protocol_, node.second, message.second));
                command->set_type(cute::proto::Handshake_Command_Type_BOOL);
            }
        }
    }

    cute::proto::PacketSP cutepacket = std::make_shared<cute::proto::Packet>();
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