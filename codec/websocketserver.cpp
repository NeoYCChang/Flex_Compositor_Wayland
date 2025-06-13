#include "websocketserver.h"

WebSocketServer::WebSocketServer(quint16 port, QObject *parent)
    : QObject(parent),
    m_server(new QWebSocketServer(QStringLiteral("Test Server"),
                                  QWebSocketServer::NonSecureMode, this)) {
    if (m_server->listen(QHostAddress::Any, port)) {
        connect(m_server, &QWebSocketServer::newConnection,
                this, &WebSocketServer::onNewConnection);
        qDebug() << "WebSocket server listening on port" << port;
    } else {
        qDebug() << "WebSocket server failed to start";
    }
}

WebSocketServer::~WebSocketServer() {
    m_server->close();
    qDeleteAll(m_clients);
}

void WebSocketServer::onNewConnection() {
    QWebSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QWebSocket::textMessageReceived,
            this, &WebSocketServer::onTextMessageReceived);
    connect(socket, &QWebSocket::binaryMessageReceived,
            this, &WebSocketServer::onBinaryMessageReceived);
    connect(socket, &QWebSocket::disconnected,
            this, &WebSocketServer::onSocketDisconnected);

    m_clients << socket;
    qDebug() << "New client connected";
}

void WebSocketServer::onTextMessageReceived(const QString &message) {
    qDebug() << "Text message received:" << message;
}

void WebSocketServer::onBinaryMessageReceived(const QByteArray &message) {
    qDebug() << "Binary message received:" << message.size() << "bytes";
}

void WebSocketServer::onSocketDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_clients.removeAll(client);
        client->deleteLater();
        qDebug() << "Client disconnected";
    }
}

void WebSocketServer::sendMessageToAll(const QByteArray &data) {
    for (QWebSocket *client : std::as_const(m_clients)) {
        if (client->isValid()) {
            client->sendBinaryMessage(data);
        }
    }
}
