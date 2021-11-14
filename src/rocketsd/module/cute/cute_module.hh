#ifndef ROCKETSD_MODULES_CUTE_CUTEMODULE_HH_
#define ROCKETSD_MODULES_CUTE_CUTEMODULE_HH_

#include <rocketsd/module/module.hh>

#include <memory>

#include <QAbstractSocket>

#include <cute/proto/packet.hh>

namespace rocketsd::modules::cute {

class CuteModule: public Module {
    Q_OBJECT
    struct Priv;

public:
    CuteModule(QObject* parent);
    ~CuteModule() override;

    bool init(json& config) override;

    std::string type() const override { return "CuteStation"; }

private:
    void connect();
    void dispatch(std::shared_ptr<::cute::proto::Packet> packet);

public slots:
    void onMessage(Message message) override;
    void onSocketData();

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    std::unique_ptr<Priv> _d;
};

} // namespace

#endif