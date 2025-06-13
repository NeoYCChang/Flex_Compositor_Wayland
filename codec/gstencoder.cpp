#include "gstencoder.h"

GstEncoder::GstEncoder(QString name, int id, Compositor* source, EGLHelper::TextureCropSize* textureCropSize, int w, int h, QObject *parent)
: QObject(parent), m_width(w), m_height(h), m_source(source), m_textureCropSize(textureCropSize), e_name(name), e_id(id)
{

    //connect(m_thread, &QThread::started, this, &GstEncoder::initialize);
    initialize();
//     m_thread = new QThread();
//     this->moveToThread(m_thread);
//     m_thread->start();
}

GstEncoder::~GstEncoder()
{
    stop();
    deleteGstEncoderThread();
    delete m_gstEncoderThread;
    delete m_offscreenSurface;
    delete m_render;
    delete m_thread;
}

QSurfaceFormat GstEncoder::getFormat()
{
    return m_offscreenSurface->requestedFormat();
}

QSurface *GstEncoder::getSurface()
{
    return m_offscreenSurface;
}

QSize GstEncoder::getSize()
{
    return QSize(m_width, m_height);
}

Compositor *GstEncoder::getSource()
{
    return m_source;
}

void GstEncoder::stop() {
    QMutexLocker locker(&mutex);
    // 可擴充為處理停止狀態
}

void GstEncoder::initialize()
{
    createGstEncoderThread();
    createOffscreenSurface();
    createRender();

    if(m_source){
        connect(m_source, &Compositor::requestUpdate, this, &GstEncoder::onRequestUpdate, Qt::DirectConnection);
    }
}

void GstEncoder::deleteGstEncoderThread()
{
    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
        delete m_gstEncoderThread;
    }
}

void GstEncoder::createGstEncoderThread()
{
    m_gstEncoderThread = new GstEncoderThread(m_width, m_height);
    m_thread = new QThread();
    m_gstEncoderThread->moveToThread(m_thread);
    m_thread->start();
}

void GstEncoder::createOffscreenSurface()
{
    m_offscreenSurface = new QOffscreenSurface;
    m_offscreenSurface->setFormat(EGLHelper::context()->format());
    m_offscreenSurface->create();
}

void GstEncoder::createRender()
{
    m_render = new EGLRender(this, EGLHelper::context(), true);
    connect(m_render, &EGLRender::imageReady, m_gstEncoderThread, &GstEncoderThread::enqueueImage,  Qt::QueuedConnection);
}

void GstEncoder::render()
{
    if(m_render && m_source){
        m_render->render();
    }
}

void GstEncoder::onRequestUpdate()
{
    render();
}
