#include "module/module_factory.hh"

#include <string>

#include <log/log.hh>
#include <util/json.hh>

#include "cute_module.hh"
#include "influx_module.hh"
#include "fake_module.hh"
#include "serial_module.hh"

namespace modules {

Module* ModuleFactory::build(rocketsd::protocol::ProtocolSP protocol, json& config, QObject* parent)
{
    Module* module;

    std::string type;
    if (!util::json::validate("ModuleFactory", config, util::json::required(type, "module"))) {
        return nullptr;
    }

    if (type == "cute") {
        module = new CuteModule(parent, protocol);
    } else if (type == "influx") {
        module = new InfluxModule(parent, protocol);
    } else if (type == "fake") {
        module = new FakeModule(parent, protocol);
    } else if (type == "serial") {
        module = new SerialModule(parent, protocol);
    } else {
        Log::warn("ModuleFactory") << "Unknown module type '" << type << "'" << std::endl;
        return nullptr;
    }

    if (!module->init(config)) {
        delete module;
        return nullptr;
    }

    return module;
}

} // namespace