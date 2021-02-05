#ifndef FAKE_MODULE_HH_
#define FAKE_MODULE_HH_

#include "module/module.hh"

#include <QTimer>

namespace modules {

class FakeModule : public Module {
    Q_OBJECT

private:
    QTimer* timer_;
    double freq_, n_, alpha_, omega_;
    unsigned int nodeid_, messageid_;

public:
    FakeModule(QObject* parent, rocketsd::protocol::ProtocolSP protocol);
    ~FakeModule() override;

    bool init(json& config) override;

    std::string type() const override { return "Fake"; }

public slots:
    void onPacket(radio_packet_t packet) override;

private slots:
    void onTimeout();
};

} // namespace

#endif