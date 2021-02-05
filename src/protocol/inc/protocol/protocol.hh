#ifndef ROCKETSD_PROTOCOL_HH_
#define ROCKETSD_PROTOCOL_HH_

#include <memory>
#include <string>
#include <unordered_map>

namespace rocketsd::protocol {

/** Message
 * Maps an Message ID to a name with a format. The internal representation of
 * the protocol is not a copy of the XML: message groups are flatenned directly
 * into Messages with a dot-delimited name (<...>.group.message)
 */
class Message {
private:
    int id_;
    std::string name_;
    std::string type_;
    bool command_;

public:
    Message(int id, const std::string& name, const std::string& type, bool command = false);
    ~Message();

    int id() const { return id_; }
    std::string_view name() const { return std::string_view(name_); }
    std::string_view type() const { return std::string_view(type_); }
    bool command() const { return command_; }
};

class Node {
private:
    int id_;
    std::string name_;

    std::unordered_map<int, Message*> messages_;

public:
    Node(int id, const std::string& name);
    ~Node();

    int id() const { return id_; }
    std::string_view name() const { return std::string_view(name_); }

    void insert(int id, Message* message) { messages_.insert({id, message}); }
    Message* operator[](int id) { return messages_.count(id) ? messages_.at(id) : nullptr; }

    std::unordered_map<int, Message*>::iterator begin() { return messages_.begin(); }
    std::unordered_map<int, Message*>::iterator end() { return messages_.end(); }

    std::unordered_map<int, Message*>::const_iterator cbegin() const { return messages_.begin(); }
    std::unordered_map<int, Message*>::const_iterator cend() const { return messages_.end(); }

    std::size_t size() { return messages_.size(); }
};

class Protocol {
private:
    std::string name_;
    std::unordered_map<int, Node*> nodes_;
public:
    Protocol(const std::string& name);
    ~Protocol();

    std::string_view name() const { return std::string_view(name_); }

    void insert(int id, Node* message) { nodes_.insert({id, message}); }
    Node* operator[](int id) { return nodes_.count(id) ? nodes_.at(id) : nullptr; }

    std::unordered_map<int, Node*>::iterator begin() { return nodes_.begin(); }
    std::unordered_map<int, Node*>::iterator end() { return nodes_.end(); }

    std::unordered_map<int, Node*>::const_iterator cbegin() const { return nodes_.cbegin(); }
    std::unordered_map<int, Node*>::const_iterator cend() const { return nodes_.cend(); }

    std::size_t size() { return nodes_.size(); }
};

typedef std::shared_ptr<Protocol> ProtocolSP;

}

#endif