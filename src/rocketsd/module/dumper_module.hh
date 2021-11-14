#ifndef ROCKETSD_MODULES_DUMPERMODULE_HH_
#define ROCKETSD_MODULES_DUMPERMODULE_HH_

#include <log/log.hh>

#include "module.hh"

namespace rocketsd::modules {

class DumperModule : public Module {
    Q_OBJECT

public:
    DumperModule(QObject* parent)
        : Module(parent)
    {
    }

    ~DumperModule() override
    {
    }

    bool init(json& config) override
    {
        return Module::init(config);
    }

    std::string type() const override { return "Dumper"; }

public slots:
        void onMessage(Message message) override
        {
            std::string value, type;
            switch (message.measurement.value_case()) {
                case ::cute::proto::Measurement::kState:
                    type = "state";
                    value = std::to_string(message.measurement.state());
                    break;
                case ::cute::proto::Measurement::kNumber:
                    type = "number";
                    value = std::to_string(message.measurement.number());
                    break;
                default:
                    type = "unknown";
                    value = "nan";
            }

            logging::debug("dump") << logging::tag{"source", message.measurement.source()} << logging::tag{"type", type} << logging::tag{"value", value} << logging::endl;
        }
};

} // namespaces

#endif