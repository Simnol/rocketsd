#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <memory>
#include <string>
#include <unordered_map>

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
    Message(int id, std::string name, std::string type, bool command = false);
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
    Node(int id, std::string name);
    ~Node();

    int id() const { return id_; }
    std::string_view name() const { return std::string_view(name_); }

    void insert(int id, Message* message) { messages_.insert({id, message}); }
    Message* operator[](int id) { return messages_.count(id) ? messages_.at(id) : nullptr; }

    std::unordered_map<int, Message*>::iterator begin() { return messages_.begin(); }
    std::unordered_map<int, Message*>::iterator end() { return messages_.end(); }
};

class Protocol {
private:
    std::unordered_map<int, Node*> nodes_;
public:
    Protocol();
    ~Protocol();

    void insert(int id, Node* message) { nodes_.insert({id, message}); }
    Node* operator[](int id) { return nodes_.count(id) ? nodes_.at(id) : nullptr; }

    std::unordered_map<int, Node*>::iterator begin() { return nodes_.begin(); }
    std::unordered_map<int, Node*>::iterator end() { return nodes_.end(); }
};

typedef std::shared_ptr<Protocol> ProtocolSP;

#endif