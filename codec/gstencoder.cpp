#include "gstencoder.h"

GstEncoder::GstEncoder(QString name, int id, iSource* source, QRect cropTextureSize, int w, int h, QObject *parent)
: QObject(parent), m_width(w), m_height(h), m_source(source), e_name(name), e_id(id)
{

    //connect(m_thread, &QThread::started, this, &GstEncoder::initialize);
    m_textureCropSize = new  EGLHelper::TextureCropSize{m_source->getSize(), cropTextureSize};
    initialize();
//     m_thread = new QThread();
//     this->moveToThread(m_thread);
//     m_thread->start();
}

GstEncoder::~GstEncoder()
{
    stop();
    delete m_gstEncoderThread;
    delete m_offscreenSurface;
    delete m_render;
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

EGLHelper::TextureCropSize *GstEncoder::getTextureCropSize()
{
    return m_textureCropSize;
}

iSource *GstEncoder::getSource()
{
    return m_source;
}

void GstEncoder::stop() {
    QMutexLocker locker(&mutex);
}

void GstEncoder::initialize()
{
    createGstEncoderThread();
    createOffscreenSurface();
    createRender();
}


void GstEncoder::createGstEncoderThread()
{
    m_gstEncoderThread = new GstEncoderThread(m_width, m_height);
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
    if(m_source){
        if (Compositor *compositor = dynamic_cast<Compositor *>(m_source)) {
            connect(compositor, &Compositor::requestUpdate, m_render, &EGLRender::render_async, Qt::QueuedConnection);
        } else {
        }
    }
}

void GstEncoder::render()
{
    if(m_render && m_source){
        m_render->render_async();
    }
}

