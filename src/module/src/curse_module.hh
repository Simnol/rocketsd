#ifndef CURSE_MODULE_HH_
#define CURSE_MODULE_HH_

#include "module/module.hh"

#include <string>

namespace modules::curses {

class CurseOutputWindow;
class CurseInputWindow;
class CurseLogStream;

class CurseModule : public Module {
private:
    CurseOutputWindow *log_, *output_;
    CurseLogStream* logstream_;
    CurseInputWindow *input_;

public:
    CurseModule(QObject* parent, rocketsd::protocol::ProtocolSP protocol);
    ~CurseModule();

    bool init(json& config) override;

    std::string type() const override { return "nCurses"; }

public slots:
    void onPacket(radio_packet_t) override;

private:
    std::pair<std::string, bool> onInput(std::string input);
};

} // namespaces

#endif