#ifndef ROCKETSD_APP_APP_HH_

#include <QCoreApplication>

#include <QThread>

#include <protocol/protocol.hh>

namespace rocketsd::app {

class App : public QCoreApplication {
private:
    protocol::ProtocolSP protocol_;
    std::vector<QThread*> workers_;

public:
    App(int argc, char* argv[]);
    ~App();
};

} // namespaces

#endif