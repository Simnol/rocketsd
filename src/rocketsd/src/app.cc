#include "app.hh"

#include <iostream>
#include <fstream>

#include <QThread>

#include <nlohmann/json.hpp>

#include <shared/interfaces/radio/radio_packet.h>

#include <log/log.hh>
#include <module/module_factory.hh>
#include <protocol/protocol_parser.hh>
#include <util/json.hh>

using json = nlohmann::json;

Q_DECLARE_METATYPE(radio_packet_t)

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
        Log::err("rocketsd", "XML protocol path cannot be empty");
        QCoreApplication::quit();
    }

    rocketsd::protocol::ProtocolParser parser;
    protocol_ = rocketsd::protocol::ProtocolSP(parser.parse(std::filesystem::path(xmlpath)));

    if (!protocol_) {
        Log::err("rocketsd", "Could not load XML protocol");
        QCoreApplication::quit();
    }

    modules::Module* first = nullptr;

    for (auto& subconfig : config["modules"]) {
        modules::Module* module;
        if (!(module = modules::ModuleFactory::build(protocol_, subconfig))) {
            continue;
        }

        QThread* thread = new QThread(this);
        thread->setObjectName(module->type().c_str());
        module->moveToThread(thread);
        connect(thread, &QThread::finished, module, &QObject::deleteLater);

        if (first) {
            connect(first, &modules::Module::packetReady, module, &modules::Module::onPacket, Qt::QueuedConnection);
            connect(module, &modules::Module::packetReady, first, &modules::Module::onPacket, Qt::QueuedConnection);
        } else {
            first = module;
        }

        thread->start();
        workers_.push_back(thread);
    }
}

App::~App()
{
    Log::info("App", "Quitting!");
    for (QThread* thread: workers_) {
        thread->quit();
    }

    for (QThread* thread: workers_) {
        thread->wait();
    }
}