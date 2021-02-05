#include "module/module.hh"

namespace modules {

Module::Module(QObject* parent, rocketsd::protocol::ProtocolSP protocol)
    : QObject(parent)
    , protocol_(protocol)
{
}

Module::~Module() {}

} // namespace