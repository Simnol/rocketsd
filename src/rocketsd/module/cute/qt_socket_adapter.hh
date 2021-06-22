#ifndef ROCKETSD_MODULES_CUTE_QTSOCKETADAPTER_HH_
#define ROCKETSD_MODULES_CUTE_QTSOCKETADAPTER_HH_

#include <memory>

#include <QAbstractSocket>
#include <QLocalSocket>
#include <QTcpSocket>

namespace rocketsd::modules::cute {

/** Wraps around QAbstractSocket and QLocalSocket to provide a unified interface
 *  (limited to the functionality we need)
 */
class QtSocketAdapter : public QObject {
    Q_OBJECT
    struct Priv;

public:
    QtSocketAdapter(QObject* parent, QAbstractSocket* socket, const std::string& host, unsigned port);
    QtSocketAdapter(QObject* parent, QLocalSocket* socket, const std::string& path);
    virtual ~QtSocketAdapter();

    bool isConnected();
    QIODevice* device();


signals:
    void connected();
    void disconnected();
    void errorOccured(QAbstractSocket::SocketError error);
    void readyRead();

private:
    std::unique_ptr<Priv> _d;

};

}

#endif