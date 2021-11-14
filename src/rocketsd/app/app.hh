#ifndef ROCKETSD_APP_APP_HH_
#define ROCKETSD_APP_APP_HH_

#include <QCoreApplication>

#include <filesystem>

#include <QThread>

#include <nlohmann/json.hpp>

namespace rocketsd::app {

class App : public QCoreApplication {
private:
    std::vector<QThread*> workers_;

public:
    App(nlohmann::json config, int argc, char* argv[]);
    ~App();
};

} // namespaces

#endif