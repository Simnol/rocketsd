#ifndef MODULE_HH_
#define MODULE_HH_

#include <QObject>

#include <nlohmann/json.hpp>

#include <protocol/protocol.hh>
#include "shared/interfaces/radio/radio_packet.h"

using json = nlohmann::json;

namespace modules {

class Module : public QObject {
    Q_OBJECT

private:
    std::string _id;

protected:
    rocketsd::protocol::ProtocolSP _protocol;

public:
    Module(QObject* parent, rocketsd::protocol::ProtocolSP protocol);
    virtual ~Module();

    virtual bool init(json& config);

    std::string id() const;
    virtual std::string type() const = 0;

public slots:
    virtual void onPacket(radio_packet_t packet) = 0;

signals:
    void packetReady(radio_packet_t packet);
};

} // namespace

#endif