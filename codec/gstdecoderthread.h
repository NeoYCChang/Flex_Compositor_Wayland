#ifndef GSTDECODERTHREAD_H
#define GSTDECODERTHREAD_H

#include <QObject>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <QSize>
#include <QThread>
#include <QUrl>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QOpenGLTexture>
#include "websocketclient.h"

class GstDecoderThread : public QObject
{
    Q_OBJECT
public:
    explicit GstDecoderThread(int w, int h,  QString ip, quint16 port, QObject *parent = nullptr);
    ~GstDecoderThread();
    void fillTexturData(QOpenGLTexture* texture);

signals:
    void decodeFrame(QByteArray data);
    void triggerRender();

private:
    GstElement *m_pipeline = nullptr;
    GstElement *m_appsrc = nullptr;
    GstElement *m_appsink = nullptr;
    int m_width = 960;
    int m_height = 540;
    gsize m_frameSize = 0;
    QByteArray* m_decodeData = nullptr;
    QThread* m_thread = nullptr;
    QMutex m_mutex;
    QUrl m_url;
    WebSocketClient* m_webSocketClient = nullptr;


private:
    static GstFlowReturn onNewSample(GstAppSink *sink, gpointer user_data);
    void createPipeLine();
    void createWebSocketClient();


private slots:
    void initialize();
    void enqueueEncodedData(const QByteArray &data);
    void onDecodeFrame(QByteArray data);
};

#endif // GSTDECODERTHREAD_H
