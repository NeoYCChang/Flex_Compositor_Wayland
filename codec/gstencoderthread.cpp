#include "gstencoderthread.h"

GstEncoderThread::GstEncoderThread(int w, int h, QObject *parent)
    : QObject{parent}, m_width(w), m_height(h)
{
    m_thread = new QThread();
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, &GstEncoderThread::initialize, Qt::DirectConnection);
    m_thread->start();
}

GstEncoderThread::~GstEncoderThread()
{
    gst_element_send_event(m_pipeline, gst_event_new_eos());
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref(m_appsrc);
    gst_object_unref(m_appsink);
    gst_object_unref(m_pipeline);
    if(m_thread){
        m_thread->requestInterruption();
        m_thread->quit();         // Asks event loop to exit (if you use one)
        m_thread->wait();         // Blocks until thread exits
    }
    delete m_thread;
}

void GstEncoderThread::enqueueImage(const QImage &image) {
    //qDebug() << "enqueueImage";
    if (image.size() != QSize(m_width, m_height)) {
        qWarning() << "Image size mismatch";
        return;
    }
    QImage copiedImage = image.copy();
    // GstBuffer *buffer = gst_buffer_new_allocate(nullptr, copiedImage.sizeInBytes(), nullptr);
    // GstMapInfo map;
    // gst_buffer_map(buffer, &map, GST_MAP_WRITE);
    // memcpy(map.data, copiedImage.bits(), copiedImage.sizeInBytes());
    // gst_buffer_unmap(buffer, &map);
    GstBuffer* buffer = gst_buffer_new_wrapped((gpointer)copiedImage.bits(),copiedImage.sizeInBytes());


    GstFlowReturn ret;
    g_signal_emit_by_name(m_appsrc, "push-buffer", buffer, &ret);
    //gst_buffer_unref(buffer);
    if (ret != GST_FLOW_OK) {
        qWarning() << "Failed to push buffer";
    }
}

GstFlowReturn GstEncoderThread::onNewSample(GstAppSink *sink, gpointer user_data)
{
    GstEncoderThread *self = static_cast<GstEncoderThread *>(user_data);
    GstSample *sample = gst_app_sink_pull_sample(sink);

    if (!sample)
        return GST_FLOW_OK;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        QByteArray encodedData((const char *)map.data, map.size);
        // int offSet = 4;

        // int type = (static_cast<unsigned char>(encodedData[offSet]) & 0x7E) >> 1;
        // qDebug()<<type;
        // // // qDebug()<<encodedData.size();


        emit self->encodedFrame(encodedData);
        gst_buffer_unmap(buffer, &map);
    }

    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void GstEncoderThread::createPipeLine()
{
    gst_init(nullptr, nullptr);

    m_pipeline = gst_parse_launch(
        "appsrc name=src ! videoconvert ! video/x-raw,format=I420 ! x265enc tune=zerolatency key-int-max=60 speed-preset=ultrafast bitrate=4150 ! "
        "h265parse config-interval=-1 ! "
        "video/x-h265, stream-format=byte-stream ! appsink name=sink emit-signals=true", nullptr);

    m_appsrc = gst_bin_get_by_name(GST_BIN(m_pipeline), "src");
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "sink");

    // set appsrc caps
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "BGRA",
                                        "width", G_TYPE_INT, m_width,
                                        "height", G_TYPE_INT, m_height,
                                        "framerate", GST_TYPE_FRACTION, 60, 1,
                                        nullptr);
    gst_app_src_set_caps(GST_APP_SRC(m_appsrc), caps);
    gst_caps_unref(caps);
    g_object_set(m_appsrc,
                 "block", FALSE, // 阻塞直到有空间
                 nullptr);

    // set appsink to use signal mode
    g_object_set(m_appsink, "emit-signals", TRUE, nullptr);
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(GstEncoderThread::onNewSample), this);

    GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qWarning() << "Pipeline failed to start";
    }
}

void GstEncoderThread::createWebSocketServer()
{
    m_webSocketServer = new WebSocketServer(50000, this);
    connect(this, &GstEncoderThread::encodedFrame, m_webSocketServer, &WebSocketServer::sendMessageToAll,  Qt::QueuedConnection);
}

void GstEncoderThread::initialize()
{
    createWebSocketServer();
    createPipeLine();
}
