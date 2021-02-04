#include "modules/fake_module.h"

#include <iostream>

#include <cmath>

#include "shared/interfaces/id.h"

#include "log.h"
#include "util.h"

namespace modules {

FakeModule::FakeModule(QObject* parent, ProtocolSP protocol)
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
    freq_ = 0;
    if (has<float>(config, "frequency")) {
        freq_ = config["frequency"].get<double>();
    }
    if (freq_ < 0) {
        freq_ = 1000;
    }

    alpha_ = 1;
    if (has<float>(config, "alpha")) {
        alpha_ = config["alpha"].get<double>();
    }

    omega_ = 1;
    if (has<float>(config, "omega")) {
        omega_ = config["omega"].get<double>();
    }

    nodeid_ = 0;
    if (has<unsigned>(config, "node_id")) {
        nodeid_ = config["node_id"].get<unsigned>();
    }

    messageid_ = 0;
    if (has<unsigned>(config, "message_id")) {
        messageid_ = config["message_id"].get<unsigned>();
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