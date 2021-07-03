#include "app.hh"

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <QThread>

#include <nlohmann/json.hpp>

#include <shared/interfaces/radio/radio_packet.h>

#include <log/log.hh>
#include <rocketsd/module/module_factory.hh>
#include <protocol/protocol_parser.hh>
#include <util/json.hh>

using json = nlohmann::json;

Q_DECLARE_METATYPE(radio_packet_t)

namespace rocketsd::app {

App::App(int argc, char* argv[])
    : QCoreApplication(argc, argv)
{
    qRegisterMetaType<radio_packet_t>();

    std::ifstream ifs("config.json");
    json config = json::parse(ifs);

    std::string xmlpath;
    if(!util::json::validate("rocketsd", config, util::json::required(xmlpath, "protocol"))) {
        QCoreApplication::quit();
    }

    if (xmlpath == "") {
        logging::err("rocketsd") << "XML protocol path cannot be empty" << logging::endl;
        QCoreApplication::quit();
    }

    protocol::ProtocolParser parser;
    protocol_ = protocol::ProtocolSP(parser.parse(std::filesystem::path(xmlpath)));

    if (!protocol_) {
        logging::err("rocketsd") <<  "Could not load XML protocol" << logging::endl;
        QCoreApplication::quit();
    }

    std::unordered_map<std::string, modules::Module*> modulesById;
    std::unordered_map<std::string, std::vector<std::string>> broadcastByModule;

    for (auto& subconfig : config["modules"]) {
        modules::Module* module;
        if (!(module = modules::ModuleFactory::build(protocol_, subconfig))) {
            continue;
        }

        modulesById[module->id()] = module;
        util::json::validate("App", subconfig, util::json::optional(broadcastByModule[module->id()], std::string("broadcast"), std::vector<std::string>{}));

        QThread* thread = new QThread(this);
        thread->setObjectName(module->type().c_str());
        module->moveToThread(thread);
        connect(thread, &QThread::finished, module, &QObject::deleteLater);

        thread->start();
        workers_.push_back(thread);
    }

    for (auto& module: modulesById) {
        for (auto target: broadcastByModule.at(module.first)) {
            logging::info("App") << "Connecting " << module.first << " to " << target << logging::endl;
            connect(module.second, &modules::Module::packetReady, modulesById[target], &modules::Module::onPacket, Qt::QueuedConnection);
        }
    }
}

App::~App()
{
    logging::info("App") << "Quitting!" << logging::endl;
    for (QThread* thread: workers_) {
        thread->quit();
    }

    for (QThread* thread: workers_) {
        thread->wait();
    }
}

} // namespaces