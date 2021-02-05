#include "protocol/protocol_parser.hh"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <QDomDocument>
#include <QStringList>

#include <log/log.hh>

namespace rocketsd::protocol {

ProtocolParser::ProtocolParser() {}

ProtocolParser::~ProtocolParser() {}

Protocol* ProtocolParser::parse(std::filesystem::path file)
{
    if (!std::filesystem::exists(file)) {
        return nullptr;
    }

    file_ = file;
    std::ifstream ifs(file);
    if (ifs.fail()) {
        return nullptr;
    }

    std::stringstream ss;
    ss << ifs.rdbuf();

    return parse(ss.str());
}

Protocol* ProtocolParser::parse(std::string xml)
{
    QDomDocument doc;
    doc.setContent(QString(xml.c_str()));

    QDomElement root = doc.documentElement();
    if (root.tagName() != "protocol") {
        return nullptr;
    }

    parseProtocol(root);
    return protocol_;
}

void ProtocolParser::parseChildren(QDomElement xml, QStringList validTags)
{
    QDomElement child = xml.firstChildElement();
    while (!child.isNull()) {
        QString tag = child.tagName();

        if (!validTags.contains(tag)) {
            Log::warn("ProtocolParser") << file_
                                        << ": unexpected element '" << qPrintable(child.tagName())
                                        << "' on line " << child.lineNumber()
                                        << ": ignoring" << std::endl;
        }

        if (tag == "message_group") {
            parseMessageGroup(child);
        } else if (tag == "message") {
            parseMessage(child);
        } else if (tag == "node") {
            parseNode(child);
        }

        child = child.nextSiblingElement();
    }
}

bool ProtocolParser::requireAttributes(QDomElement xml, QStringList keys)
{
    QDomNamedNodeMap attrib = xml.attributes();

    for (const auto& key : keys) {
        if (!attrib.contains(key)) {
            Log::warn("ProtocolParser") << file_
                                        << ": missing mandatory attribute '" << qPrintable(key)
                                        << " for element '" << qPrintable(xml.tagName())
                                        << "' on line " << xml.lineNumber()
                                        << ": skipping. " << std::endl;
            return false;
        }
    }

    return true;
}

void ProtocolParser::parseNode(QDomElement xml)
{
    if (!requireAttributes(xml, { "id", "name" })) {
        return;
    }

    bool ok = true;
    int id = xml.attribute("id").toInt(&ok);
    if (!ok) {
        Log::warn("ProtocolParser") << file_ << ": 'id' attribute not integer for 'node' element on line " << xml.lineNumber() << ": skipping" << std::endl;
        return;
    }

    current_node_ = new Node(id, xml.attribute("name").toStdString());
    current_name_tokens_.clear();
    protocol_->insert(id, current_node_);

    parseChildren(xml, { "message", "message_group" });
}

void ProtocolParser::parseMessageGroup(QDomElement xml)
{
    if (!requireAttributes(xml, { "name" })) {
        return;
    }

    current_name_tokens_.push_back(xml.attribute("name").toStdString());
    parseChildren(xml, { "message", "message_group" });
    current_name_tokens_.pop_back();
}

void ProtocolParser::parseMessage(QDomElement xml)
{
    if (!requireAttributes(xml, { "id", "name", "type" })) {
        return;
    }

    bool ok = true;
    int id = xml.attribute("id").toInt(&ok);
    if (!ok) {
        Log::warn("ProtocolParser") << file_ << ": 'id' attribute not integer for 'message' element on line " << xml.lineNumber() << ": skipping" << std::endl;
        return;
    }

    std::string name = "";
    for (const auto& token : current_name_tokens_) {
        name += token + ".";
    }
    name += xml.attribute("name").toStdString();

    bool command = false;
    if (xml.hasAttribute("command") && xml.attribute("command").toStdString() == "yes") {
        command = true;
    }

    current_node_->insert(id, new Message(id, name, xml.attribute("type").toStdString(), command));
}

void ProtocolParser::parseProtocol(QDomElement xml)
{
    std::string name = "";
    if (xml.hasAttribute("name")) {
        name = xml.attribute("name").toStdString();
    }

    protocol_ = new Protocol(name);
    parseChildren(xml, { "node" });
}

}