#include "app.hh"

#include <fstream>

#include <QThread>

#include <nlohmann/json.hpp>

#include <shared/interfaces/radio/radio_packet.h>

#include <log/log.hh>
#include <protocol/protocol_parser.hh>
#include <module/module_factory.hh>

using json = nlohmann::json;

Q_DECLARE_METATYPE(radio_packet_t)

App::App(int argc, char* argv[])
    : QCoreApplication(argc, argv)
{
    qRegisterMetaType<radio_packet_t>();

    rocketsd::protocol::ProtocolParser parser;
    protocol_ = rocketsd::protocol::ProtocolSP(parser.parse(std::filesystem::path("protocol.xml")));

    if (!protocol_) {
        return;
    }

    std::ifstream ifs("config.json");
    json config = json::parse(ifs);

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