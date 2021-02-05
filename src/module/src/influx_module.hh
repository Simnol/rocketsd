#ifndef INFLUX_MODULE_HH_
#define INFLUX_MODULE_HH_

#include "module/module.hh"

#include <sstream>

#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace modules {

class InfluxModule : public Module {
    Q_OBJECT

private:
    QNetworkAccessManager* network_;
    std::string base_url_;
    int max_lines_;

    std::stringstream buffer_;
    int lines_;

public:
    InfluxModule(QObject* parent, rocketsd::protocol::ProtocolSP protocol);
    ~InfluxModule() override;

    bool init(json& config) override;

    std::string type() const override { return "InfluxDB"; }

public slots:
    void onPacket(radio_packet_t packet) override;

private slots:
    void onError(QNetworkReply* reply);
};

} // namespaces

#endif