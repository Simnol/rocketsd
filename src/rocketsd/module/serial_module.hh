#ifndef ROCKETSD_MODULES_SERIALMODULE_HH_
#define ROCKETSD_MODULES_SERIALMODULE_HH_

#include <rocketsd/module/module.hh>

#include <deque>

#include <QtSerialPort>

namespace rocketsd::modules {

class SerialModule : public Module {
private:
    QSerialPort* serialport_;

    std::deque<std::uint8_t> buffer_;

public:
    SerialModule(QObject* parent, protocol::ProtocolSP protocol);
    ~SerialModule() override;

    bool init(json& config) override;

    std::string type() const override { return "Serial"; }

public slots:
    void onPacket(radio_packet_t packet) override;

private slots:
    void onError(QSerialPort::SerialPortError error);
    void onData();
};

}

#endif