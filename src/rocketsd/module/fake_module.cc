#include <iostream>
#include <numbers>

#include <cmath>

#include <log/log.hh>
#include <util/json.hh>

#include "fake_module.hh"

namespace rocketsd::modules {

FakeModule::FakeModule(QObject* parent)
    : Module(parent)
{
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &FakeModule::onTimeout);
    n_ = 0;
}

FakeModule::~FakeModule()
{
}

bool FakeModule::init(json& config)
{
    if (!Module::init(config)) {
        return false;
    }

    if (!util::json::validate("FakeModule", config,
        util::json::optional(freq_, "frequency", 1.0),
        util::json::optional(alpha_, "alpha", 1.0),
        util::json::optional(omega_, "omega", 1.0),
        util::json::optional(phi_, "phi", 0.0),
        util::json::required(mid_, "mid")
    )) {
        return false;
    }

    timer_->start(1000 / freq_);

    logging::info("FakeModule") << "Successfully init'd Fake producer" << logging::tag{"id", id()} << logging::endl;
    return true;
}

void FakeModule::onTimeout()
{
    cute::proto::Measurement measurement;
    measurement.set_source(mid_);
    measurement.set_number(alpha_ * std::sin(n_) + phi_);
    n_ += omega_ * ((std::numbers::pi * 2) / freq_);
    emit messageReady({this, measurement});
}

void FakeModule::onMessage(Message message)
{
    logging::info("FakeModule") << "Received measurement: " << message.measurement.source() << logging::endl;
}

} // namespace