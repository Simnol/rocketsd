#include "protocol/protocol.hh"

namespace rocketsd::protocol {

Message::Message(int id, const std::string& name, const std::string& type, bool command)
    : id_(id)
    , name_(name)
    , type_(type)
    , command_(command)
{
}

Message::~Message() {}

Node::Node(int id, const std::string& name)
    : id_(id)
    , name_(name)
{
}

Node::~Node()
{
    for (auto& message: *this) {
        delete message.second;
    }
}

Protocol::Protocol(const std::string& name)
    : name_(name)
{
}

Protocol::~Protocol()
{
    for (auto& node: *this) {
        delete node.second;
    }
}

}