#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>

class WebSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketClient(const QUrl &url, QObject *parent = nullptr);
    ~WebSocketClient();

signals:
    void recieveByteArray(const QByteArray &message);

private:
    QWebSocket *m_webSocket;
    QUrl m_url;
    QTimer m_retryTimer;


private slots:
    void onConnected();
    void onRetryConnection();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
};

#endif // WEBSOCKETCLIENT_H
