#include <rocketsd/module/module.hh>

#include <util/json.hh>

namespace rocketsd::modules {

Module::Module(QObject* parent, protocol::ProtocolSP protocol)
    : QObject(parent)
    , protocol_(protocol)
{
}

bool Module::init(json& config)
{
    return util::json::validate("Module", config,
        util::json::required(id_, "id")
    );
}

std::string Module::id() const
{
    return id_;
}

Module::~Module() = default;

} // namespace