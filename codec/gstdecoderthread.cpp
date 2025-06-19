#include "gstdecoderthread.h"

GstDecoderThread::GstDecoderThread(int w, int h, QString ip, quint16 port, QObject *parent)
    : QObject{parent}, m_width(w), m_height(h)
{
    m_thread = new QThread();
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, &GstDecoderThread::initialize, Qt::DirectConnection);
    m_thread->start();
    m_url = QUrl(QString("ws://%1:%2").arg(ip, QString::number(port)));
}

GstDecoderThread::~GstDecoderThread()
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
    delete m_webSocketClient;
    m_decodeData->clear();
    delete m_decodeData;
}

void GstDecoderThread::fillTexturData(QOpenGLTexture *texture)
{
    QMutexLocker locker(&m_mutex);
    if(texture && m_decodeData){
        texture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, m_decodeData->data());
    }
}

void GstDecoderThread::enqueueEncodedData(const QByteArray &data)
{
//    qDebug()<<"enqueueEncodedData"<<data.size();
   GstBuffer* buffer = gst_buffer_new_allocate(nullptr, data.size(), nullptr);
   GstMapInfo map;
   gst_buffer_map(buffer, &map, GST_MAP_WRITE);
   memcpy(map.data, data.constData(), data.size());
   gst_buffer_unmap(buffer, &map);

    // memcpy(m_frameData, data.constData(), data.size());
    // GstBuffer *buffer = gst_buffer_new_wrapped_full(
    //     GST_MEMORY_FLAG_READONLY,
    //     m_frameData,
    //     data.size(),
    //     0,
    //     data.size(),
    //     nullptr, // no custom free function
    //     nullptr
    //     );

    GstFlowReturn ret;
    g_signal_emit_by_name(m_appsrc, "push-buffer", buffer, &ret);
    gst_buffer_unref(buffer);
    if (ret != GST_FLOW_OK) {
        qWarning() << "Failed to push buffer";
    }
}

void GstDecoderThread::onDecodeFrame(QByteArray data)
{
    QMutexLocker locker(&m_mutex);
    if(data.size() == m_frameSize){
        memcpy(m_decodeData->data(), data.data(), data.size());
        emit triggerRender();
    }
}

GstFlowReturn GstDecoderThread::onNewSample(GstAppSink *sink, gpointer user_data)
{
    GstDecoderThread *self = static_cast<GstDecoderThread *>(user_data);
    GstSample *sample = gst_app_sink_pull_sample(sink);

    if (!sample)
        return GST_FLOW_OK;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        QByteArray decodedData = QByteArray::fromRawData((const char *)map.data, map.size);
        // int offSet = 4;

        // int type = (static_cast<unsigned char>(encodedData[offSet]) & 0x7E) >> 1;
//       qDebug()<<encodedData.size();
//        QImage image(reinterpret_cast<const uchar *>(map.data), 1920, 1080, QImage::Format_RGBA8888);
//        image.save("./imag.png");
        // // // qDebug()<<encodedData.size();


        emit self->decodeFrame(decodedData);
        gst_buffer_unmap(buffer, &map);
    }

    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void GstDecoderThread::createPipeLine()
{
    gst_init(nullptr, nullptr);

    m_pipeline = gst_parse_launch(
        "appsrc name=src  is-live=true ! h265parse config-interval=-1 ! avdec_h265 ! videoconvert ! video/x-raw,format=RGBA ! "
        "appsink name=sink emit-signals=true", nullptr);

    m_appsrc = gst_bin_get_by_name(GST_BIN(m_pipeline), "src");
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "sink");

    // set appsrc caps
    GstCaps *caps = gst_caps_new_simple("video/x-h265",
                                        "stream-format", G_TYPE_STRING, "byte-stream",
                                        "alignment", G_TYPE_STRING, "au",
                                        nullptr);
    gst_app_src_set_caps(GST_APP_SRC(m_appsrc), caps);
    gst_caps_unref(caps);
    g_object_set(m_appsrc,
                 "block", FALSE,
                 "do-timestamp", TRUE,
                 nullptr);

    // set appsink to use signal mode
    g_object_set(m_appsink, "emit-signals", TRUE, nullptr);
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(GstDecoderThread::onNewSample), this);

    GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qWarning() << "Pipeline failed to start";
    }
    m_frameSize = m_width * m_height * 4; // BGRA = 4 bytes per pixel
    m_decodeData = new QByteArray(m_frameSize, Qt::Initialization::Uninitialized);
    connect(this, &GstDecoderThread::decodeFrame, this, &GstDecoderThread::onDecodeFrame, Qt::QueuedConnection);
}

void GstDecoderThread::createWebSocketClient()
{
    m_webSocketClient = new WebSocketClient(m_url, this);
    connect(m_webSocketClient, &WebSocketClient::recieveByteArray, this, &GstDecoderThread::enqueueEncodedData, Qt::QueuedConnection);
}

void GstDecoderThread::initialize()
{
    createPipeLine();
    createWebSocketClient();
}
