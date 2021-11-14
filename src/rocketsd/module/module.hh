#ifndef ROCKETSD_MODULES_MODULE_HH_
#define ROCKETSD_MODULES_MODULE_HH_

#include <QObject>

#include <nlohmann/json.hpp>

#include <rocketsd/protocol/protocol.hh>
#include <shared/interfaces/radio/radio_packet.h>

#include "message.hh"

using json = nlohmann::json;

namespace rocketsd::modules {

class Module : public QObject {
    Q_OBJECT

protected:
    std::string id_;

public:
    Module(QObject* parent);
    virtual ~Module();

    virtual bool init(json& config);

    std::string id() const;
    virtual std::string type() const = 0;

public slots:
    virtual void onMessage(Message message) = 0;

signals:
    void messageReady(Message message);
};

} // namespace

#endif