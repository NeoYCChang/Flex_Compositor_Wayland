#include "websocketclient.h"

WebSocketClient::WebSocketClient(const QUrl &url, QObject *parent)
    : QObject{parent}, m_url(url)
{
    m_webSocket = new QWebSocket("test client", QWebSocketProtocol::VersionLatest, this);
    // connect(m_webSocket, &QWebSocket::error,
    //         this, &WebSocketClient::onError);
    connect(m_webSocket, &QWebSocket::connected,
            this, &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived,
            this, &WebSocketClient::onBinaryMessageReceived);
    connect(m_webSocket, &QWebSocket::disconnected,
            this, &WebSocketClient::onDisconnected);

    m_retryTimer.setInterval(5000);
    m_retryTimer.setSingleShot(true);
    connect(&m_retryTimer, &QTimer::timeout,
            this, &WebSocketClient::onRetryConnection);
    m_webSocket->open(url);

}

WebSocketClient::~WebSocketClient()
{
    m_webSocket->close();
}

void WebSocketClient::onConnected()
{
    qDebug() << "Connect to" << m_url;
}

void WebSocketClient::onRetryConnection()
{
    qDebug() << "Retrying connection...";
    if (m_webSocket->state() == QAbstractSocket::UnconnectedState) {
        m_webSocket->open(m_url);
    }
}

void WebSocketClient::onDisconnected()
{
    qDebug() << "WebSocket error:" << m_webSocket->error();

    if (m_webSocket->state() != QAbstractSocket::ConnectedState &&
        !m_retryTimer.isActive() && m_webSocket->error() != QAbstractSocket::UnknownSocketError) {
        m_retryTimer.start();
    }
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{

}

void WebSocketClient::onBinaryMessageReceived(const QByteArray &message)
{
    emit recieveByteArray(message);
}

