#include <rocketsd/module/module_factory.hh>

#include <string>

#include <log/log.hh>
#include <util/json.hh>


#include <rocketsd/module/cute/cute_module.hh>
#include <rocketsd/module/fake_module.hh>
#include <rocketsd/module/influx_module.hh>
#include <rocketsd/module/serial_module.hh>
#include <rocketsd/module/dumper_module.hh>

namespace rocketsd::modules {

Module* ModuleFactory::build(json& config, QObject* parent)
{
    Module* module;

    std::string type;
    if (!util::json::validate("ModuleFactory", config, util::json::required(type, "module"))) {
        return nullptr;
    }

    if (type == "cute") {
        module = new cute::CuteModule(parent);
    } else if (type == "influx") {
        module = new InfluxModule(parent);
    } else if (type == "fake") {
        module = new FakeModule(parent);
    } else if (type == "serial") {
        module = new SerialModule(parent);
    } else if (type == "dumper") {
        module = new DumperModule(parent);
    } else {
        logging::warn("ModuleFactory") << "Unknown module type '" << type << "'" << logging::endl;
        return nullptr;
    }

    if (!module->init(config)) {
        delete module;
        return nullptr;
    }

    return module;
}

} // namespace