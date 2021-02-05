#include "protocol/name.hh"

#include <sstream>

namespace rocketsd::protocol {

template <class T, class U>
T* find(const U* container, const std::string& name)
{
    const auto iter = std::find_if(container->cbegin(), container->cend(), [name](const auto item) { return item.second->name() == name; });

    if (iter != container->cend()) {
        return iter->second;
    }
    return nullptr;
}

std::string to_cute_name(const Protocol* protocol, const int node_id, const int message_id)
{
    for (auto nit = protocol->cbegin(); nit != protocol->cend(); nit++) {
        if (nit->first == node_id) {
            for (auto mit = (*nit).second->cbegin(); mit != (*nit).second->cend(); mit++) {
                if (mit->first == message_id) {
                    return to_cute_name(protocol, nit->second, mit->second);
                }
            }
        }
    }

    return "";
}

std::string to_cute_name(const Protocol* protocol, const Node* node, const Message* message)
{
    std::stringstream ss;
    ss << protocol->name() << "." << node->name() << "." << message->name();
    return ss.str();
}

/** from_cute_name
 * Parse a cute name ("protocol.node.message") to find the nodes and messages
 * for a given protocol. Protocol name can be omitted from cute name.
 */
bool from_cute_name(const Protocol* protocol, const std::string& name, Node** node, Message** message)
{
    *node = nullptr;
    *message = nullptr;

    std::stringstream ss(name);
    std::string token;

    if (!std::getline(ss, token, '.')) {
        return false;
    }

    if (token == protocol->name()) {
        if (!std::getline(ss, token, '.')) {
            return false;
        }
    }

    *node = find<rocketsd::protocol::Node, rocketsd::protocol::Protocol>(protocol, token);
    if (!node) {
        *node = nullptr;
        return false;
    }

    token = name.substr(ss.tellg());
    *message = find<rocketsd::protocol::Message, rocketsd::protocol::Node>(*node, token);
    if (!message) {
        *node = nullptr;
        *message = nullptr;
        return false;
    }

    return true;
}

}