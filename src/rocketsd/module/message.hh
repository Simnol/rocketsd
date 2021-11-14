#ifndef ROCKETSD_MODULE_MESSAGE_HH_
#define ROCKETSD_MODULE_MESSAGE_HH_

#include <cute/proto/packet.hh>

namespace rocketsd::modules {

class Module;

struct Message {
    Module* source;
    cute::proto::Measurement measurement;
};

}

#endif