#include <iostream>
#include <fstream>
#include <unordered_map>

#include <QThread>

#include <nlohmann/json.hpp>

#include <shared/interfaces/radio/radio_packet.h>

#include <log/log.hh>
#include <rocketsd/module/module_factory.hh>
#include <util/json.hh>

#include "app.hh"

using json = nlohmann::json;

Q_DECLARE_METATYPE(rocketsd::modules::Message)

namespace rocketsd::app {

App::App(json config, int argc, char* argv[])
    : QCoreApplication(argc, argv)
{
    qRegisterMetaType<rocketsd::modules::Message>();

    std::unordered_map<std::string, modules::Module*> modulesById;
    std::unordered_map<std::string, std::vector<std::string>> broadcastByModule;

    for (auto& subconfig : config["modules"]) {
        modules::Module* module;
        if (!(module = modules::ModuleFactory::build(subconfig))) {
            logging::err("App") << "Failed to create module!" << logging::endl;
            continue;
        }

        if (modulesById.count(module->id())) {
            logging::err("App") << "There is already a module with ID " << module->id() << logging::endl;
            delete module;
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
            if (modulesById[target]) {
                logging::debug("App") << "Connecting " << module.first << " to " << target << logging::endl;
                connect(module.second, &modules::Module::messageReady, modulesById[target], &modules::Module::onMessage, Qt::QueuedConnection);
            } else {
                logging::err("App") << "Could not connect " << module.first << " to " << target << ": it doesn't exist" << logging::endl;
            }
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