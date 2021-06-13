#include "module/module.hh"

#include <util/json.hh>

namespace modules {

Module::Module(QObject* parent, rocketsd::protocol::ProtocolSP protocol)
    : QObject(parent)
    , _protocol(protocol)
{
}

Module::~Module()
{
}

bool Module::init(json& config)
{
    return util::json::validate(config,
        util::json::required(_id, "id")
    );
}

std::string Module::id() const
{
    return _id;
}

} // namespace