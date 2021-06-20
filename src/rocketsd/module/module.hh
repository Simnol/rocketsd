#ifndef ROCKETSD_MODULES_MODULE_HH_
#define ROCKETSD_MODULES_MODULE_HH_

#include <QObject>

#include <nlohmann/json.hpp>

#include <protocol/protocol.hh>
#include "shared/interfaces/radio/radio_packet.h"

using json = nlohmann::json;

namespace rocketsd::modules {

class Module : public QObject {
    Q_OBJECT

protected:
    protocol::ProtocolSP protocol_;

public:
    Module(QObject* parent, protocol::ProtocolSP protocol);
    virtual ~Module();

    virtual bool init(json& config) = 0;

    virtual std::string type() const = 0;

public slots:
    virtual void onPacket(radio_packet_t packet) = 0;

signals:
    void packetReady(radio_packet_t packet);
};

} // namespace

#endif