#ifndef CUTE_MODULE_HH_
#define CUTE_MODULE_HH_

#include "module/module.hh"

#include <QLocalSocket>

#include <proto/packet.hh>

namespace modules {

class CuteModule: public Module {
    Q_OBJECT

private:
    QLocalSocket* socket_;
    std::string socket_path_;

public:
    CuteModule(QObject* parent, rocketsd::protocol::ProtocolSP protocol);
    ~CuteModule() override;

    bool init(json& config) override;

    std::string type() const override { return "CuteStation"; }

private:
    void connect();
    void dispatch(std::shared_ptr<cute::proto::Packet> packet);

public slots:
    void onPacket(radio_packet_t packet) override;
    void onSocketData();

private slots:
    void onConnected();
    void onError(QLocalSocket::LocalSocketError error);
};

} // namespace

#endif