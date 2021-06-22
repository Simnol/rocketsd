#include "qt_socket_adapter.hh"

namespace rocketsd::modules::cute {

struct QtSocketAdapter::Priv {
    QAbstractSocket* net_socket = nullptr;
    QLocalSocket* local_socket = nullptr;
};

QtSocketAdapter::QtSocketAdapter(QObject* parent, QAbstractSocket* socket, const std::string& host, unsigned port)
    : QObject(parent)
    , _d(new Priv)
{
    socket->setParent(this);

    connect(socket, &QAbstractSocket::connected, this, [this]() { emit connected(); });
    connect(socket, &QAbstractSocket::disconnected, this, [this]() { emit disconnected(); });
    connect(socket, &QAbstractSocket::errorOccurred, this, [this](auto error) { emit errorOccured(error); });
    connect(socket, &QAbstractSocket::readyRead, this, [this]() { emit readyRead(); });

    socket->connectToHost(QString::fromStdString(host), port);
    _d->net_socket = socket;
}

QtSocketAdapter::QtSocketAdapter(QObject* parent, QLocalSocket* socket, const std::string& path)
    : QObject(parent)
    , _d(new Priv)
{
    socket->setParent(this);
    
    connect(socket, &QLocalSocket::connected, this, [this]() { emit connected(); });
    connect(socket, &QLocalSocket::disconnected, this, [this]() { emit disconnected(); });
    connect(socket, &QLocalSocket::errorOccurred, this, [this](auto error) { 
            emit errorOccured(static_cast<QAbstractSocket::SocketError>(error));
    });
    connect(socket, &QLocalSocket::readyRead, this, [this]() { emit readyRead(); });

    socket->connectToServer(QString::fromStdString(path));
    _d->local_socket = socket;
}

QtSocketAdapter::~QtSocketAdapter()
{
    if (_d->net_socket) {
        _d->net_socket->disconnectFromHost();
        _d->net_socket->deleteLater();
    }
    if (_d->local_socket) {
        _d->local_socket->disconnectFromServer();
        _d->local_socket->deleteLater();
    }
}

bool QtSocketAdapter::isConnected()
{
    if (_d->net_socket) {
        return (_d->net_socket->state() == QAbstractSocket::ConnectedState);
    } else if (_d->local_socket) {
        return (_d->local_socket->state() == QLocalSocket::ConnectedState);
    } else {
        return false;
    }
}

QIODevice* QtSocketAdapter::device()
{
    if (_d->net_socket) {
        return _d->net_socket;
    } else if (_d->local_socket) {
        return _d->local_socket;
    } else {
        assert(false);
        return nullptr;
    }
}

}