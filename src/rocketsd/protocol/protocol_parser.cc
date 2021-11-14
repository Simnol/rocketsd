#include "protocol_parser.hh"

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

    file_ = file.string();
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
    if (!doc.setContent(QString(xml.c_str()))) {
        return nullptr;
    }

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
            logging::warn("ProtocolParser")
                << "ignoring unexpected element '" << qPrintable(child.tagName()) << "'" 
                << logging::tag{"line", child.lineNumber()}
                << logging::tag{"file", file_}
                << logging::endl;
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
            logging::warn("ProtocolParser")
                << "missing mandatory attribute '" << qPrintable(key) << "'" 
                << logging::tag{"line", xml.lineNumber()}
                << logging::tag{"file", file_}
                << logging::endl;

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
        logging::warn("ProtocolParser")
                << "'id' attribute not integer for 'node' element: skipping"
                << logging::tag{"line", xml.lineNumber()}
                << logging::tag{"file", file_}
                << logging::endl;
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
                logging::warn("ProtocolParser")
                << "'id' attribute not integer for 'message' element: skipping"
                << logging::tag{"line", xml.lineNumber()}
                << logging::tag{"file", file_}
                << logging::endl;
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