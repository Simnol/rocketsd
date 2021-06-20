#include <rocketsd/module/module.hh>

namespace rocketsd::modules {

Module::Module(QObject* parent, protocol::ProtocolSP protocol)
    : QObject(parent)
    , protocol_(protocol)
{
}

Module::~Module() {}

} // namespace