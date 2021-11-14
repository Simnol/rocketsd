#ifndef ROCKETSD_MODULES_SERIALMODULE_HH_
#define ROCKETSD_MODULES_SERIALMODULE_HH_

#include <deque>

#include <QtSerialPort>

#include <rocketsd/module/module.hh>
#include <rocketsd/protocol/protocol.hh>

namespace rocketsd::modules {

class SerialModule : public Module {
private:
    QSerialPort* serialport_;
    protocol::ProtocolSP protocol_;

    std::deque<std::uint8_t> buffer_;

public:
    SerialModule(QObject* parent);
    ~SerialModule() override;

    bool init(json& config) override;

    std::string type() const override { return "Serial"; }

public slots:
    void onMessage(Message message) override;

private slots:
    void onError(QSerialPort::SerialPortError error);
    void onData();
};

}

#endif