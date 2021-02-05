#ifndef MODULE_FACTORY_HH_
#define MODULE_FACTORY_HH_

#include "module.hh"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace modules {

class ModuleFactory {
public:
    static Module* build(rocketsd::protocol::ProtocolSP protocol, json& config, QObject* parent = nullptr);
};

}

#endif