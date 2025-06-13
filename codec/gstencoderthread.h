#ifndef GSTENCODERTHREAD_H
#define GSTENCODERTHREAD_H

#include <QObject>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <QImage>
#include <QDebug>
#include <QSize>
#include "websocketserver.h"


class GstEncoderThread : public QObject
{
    Q_OBJECT
public:
    explicit GstEncoderThread(int w, int h, QObject *parent = nullptr);
    ~GstEncoderThread();

public slots:
    void enqueueImage(const QImage &image);

signals:
    void encodedFrame(QByteArray data);

private:
    GstElement *m_pipeline = nullptr;
    GstElement *m_appsrc = nullptr;
    GstElement *m_appsink = nullptr;
    int m_width = 960;
    int m_height = 540;
    WebSocketServer *m_webSocketServer = nullptr;

private:
    static GstFlowReturn onNewSample(GstAppSink *sink, gpointer user_data);
    void createPipeLine();
    void createWebSocketServer();

private slots:
    void initialize();
};

#endif // GSTENCODERTHREAD_H
