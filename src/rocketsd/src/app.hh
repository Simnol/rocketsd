#ifndef ROCKETSD_APP_HH_
#define ROCKETSD_APP_HH_

#include <QCoreApplication>

#include <QThread>

#include <protocol/protocol.hh>

class App : public QCoreApplication {
private:
    rocketsd::protocol::ProtocolSP protocol_;
    std::vector<QThread*> workers_;

public:
    App(int argc, char* argv[]);
    ~App();
};

#endif