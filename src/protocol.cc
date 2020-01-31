#include "protocol.h"

Message::Message(int id, std::string name, std::string type)
    : id_(id)
    , name_(name)
    , type_(type)
{
}

Message::~Message() {}

Node::Node(int id, std::string name)
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

Protocol::Protocol() { }

Protocol::~Protocol()
{
    for (auto& node: *this) {
        delete node.second;
    }
}