#ifndef ROCKETSD_MODULES_MODULEFACTORY_HH_
#define ROCKETSD_MODULES_MODULEFACTORY_HH_

#include <rocketsd/module/module.hh>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace rocketsd::modules {

class ModuleFactory {
public:
    static Module* build(json& config, QObject* parent = nullptr);
};

}

#endif