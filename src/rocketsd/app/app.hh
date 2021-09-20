#ifndef ROCKETSD_APP_APP_HH_

#include <QCoreApplication>

#include <filesystem>

#include <QThread>

#include <protocol/protocol.hh>

namespace rocketsd::app {

class App : public QCoreApplication {
private:
    protocol::ProtocolSP protocol_;
    std::vector<QThread*> workers_;

public:
    App(const std::filesystem::path& config_path, const std::filesystem::path& xml_path, int argc, char* argv[]);
    ~App();
};

} // namespaces

#endif