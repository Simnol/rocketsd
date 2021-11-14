#ifndef ROCKETSD_APP_APP_HH_
#define ROCKETSD_APP_APP_HH_

#include <QCoreApplication>

#include <filesystem>

#include <QThread>

#include <nlohmann/json.hpp>

#include <rocketsd/protocol/protocol.hh>

namespace rocketsd::app {

class App : public QCoreApplication {
private:
    protocol::ProtocolSP protocol_;
    std::vector<QThread*> workers_;

public:
    App(nlohmann::json config, protocol::ProtocolSP procotol, int argc, char* argv[]);
    ~App();
};

} // namespaces

#endif