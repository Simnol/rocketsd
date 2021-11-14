#ifndef ROCKETSD_PROTOCOL_NAME
#define ROCKETSD_PROTOCOL_NAME

#include <string>

#include "protocol.hh"

namespace rocketsd::protocol {

std::string to_cute_name(const Protocol* protocol, int node_id, int message_id);
std::string to_cute_name(const Protocol* protocol, const Node* node, const Message* message);

bool from_cute_name(const Protocol* protocol, const std::string& name, Node** node, Message** message);

}

#endif