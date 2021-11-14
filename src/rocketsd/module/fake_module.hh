#ifndef ROCKETSD_MODULES_FAKEMODULE_HH_
#define ROCKETSD_MODULES_FAKEMODULE_HH_

#include <rocketsd/module/module.hh>

#include <QTimer>

namespace rocketsd::modules {

class FakeModule : public Module {
    Q_OBJECT

private:
    QTimer* timer_;
    double freq_, n_, alpha_, omega_, phi_;
    std::string mid_;

public:
    FakeModule(QObject* parent);
    ~FakeModule() override;

    bool init(json& config) override;

    std::string type() const override { return "Fake"; }

public slots:
        void onMessage(Message message) override;

private slots:
    void onTimeout();
};

} // namespace

#endif