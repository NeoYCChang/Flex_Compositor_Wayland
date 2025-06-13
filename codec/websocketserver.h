#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>

#include <QWebSocketServer>
#include <QWebSocket>

class WebSocketServer : public QObject {
    Q_OBJECT
public:
    explicit WebSocketServer(quint16 port, QObject *parent = nullptr);
    ~WebSocketServer();

public slots:
    void sendMessageToAll(const QByteArray &data);  // 可用來傳送 H.265 QByteArray

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onSocketDisconnected();

private:
    QWebSocketServer *m_server;
    QList<QWebSocket*> m_clients;
};

#endif // WEBSOCKETSERVER_H
