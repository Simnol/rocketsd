#ifndef ROCKETSD_PROTOCOL_PARSER_HH_
#define ROCKETSD_PROTOCOL_PARSER_HH_

#include "protocol.hh"

#include <filesystem>
#include <string>
#include <vector>

#include <QDomElement>
#include <QStringList>

namespace rocketsd::protocol {

class ProtocolParser {
private:
    std::string file_;
    Protocol* protocol_;
    Node* current_node_;
    std::vector<std::string> current_name_tokens_;

public:
    ProtocolParser();
    ~ProtocolParser();

    Protocol* parse(std::string xml);
    Protocol* parse(std::filesystem::path file);

private:
    void parseChildren(QDomElement xml, QStringList validTags);
    bool requireAttributes(QDomElement xml, QStringList keys);

    void parseNode(QDomElement xml);
    void parseMessage(QDomElement xml);
    void parseMessageGroup(QDomElement xml);
    void parseProtocol(QDomElement xml);
};

}

#endif