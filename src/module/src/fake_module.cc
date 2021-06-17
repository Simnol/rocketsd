#include "fake_module.hh"

#include <iostream>

#include <cmath>

#include "shared/interfaces/id.h"

#include <log/log.hh>
#include <util/json.hh>

#ifndef M_PI
#define M_PI 3.1415
#endif

namespace modules {

FakeModule::FakeModule(QObject* parent, rocketsd::protocol::ProtocolSP protocol)
    : Module(parent, protocol)
{
    timer_ = new QTimer();
    connect(timer_, &QTimer::timeout, this, &FakeModule::onTimeout);
    n_ = 0;
}

FakeModule::~FakeModule()
{
}

bool FakeModule::init(json& config)
{
    if (!util::json::validate("FakeModule", config,
        util::json::optionnal(freq_, "frequency", 1.0),
        util::json::optionnal(alpha_, "alpha", 1.0),
        util::json::optionnal(omega_, "omega", 1.0),
        util::json::optionnal(nodeid_, "node_id", 0u),
        util::json::optionnal(messageid_, "message_id", 0u)
    )) {
        return false;
    }

    timer_->start(1000 / freq_);

    Log::info("FakeModule") << "Successfully init'd Fake producer" << std::endl;
    return true;
}

void FakeModule::onTimeout()
{
    radio_packet_t packet;
    packet.node = nodeid_;
    packet.message_id = messageid_;
    packet.payload.FLOAT = alpha_ * std::sin(n_);
    n_ += omega_ * ((M_PI * 2) / freq_);
    emit packetReady(packet);
}

void FakeModule::onPacket(radio_packet_t packet)
{
    Log::info("FakeModule") << "Received packet: node=" << +packet.node << " message=" << +packet.message_id << " crc=" << +packet.checksum << std::endl;
}

} // namespace