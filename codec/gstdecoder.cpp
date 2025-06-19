#include "gstdecoder.h"

GstDecoder::GstDecoder(QString name, int id,  int w, int h, QString ip, quint16 port)
    : e_name(name), e_id(id),  m_width(w), m_height(h), m_ip(ip), m_port(port)
{
    initialize();
}

GstDecoder::~GstDecoder()
{
    delete m_offscreenSurface;
    delete m_eglCompositorTexture;
    delete m_gstDecoderThread;
}

QSurfaceFormat GstDecoder::getFormat()
{
    return m_offscreenSurface->requestedFormat();
}

QSurface *GstDecoder::getSurface()
{
    return m_offscreenSurface;
}

QSize GstDecoder::getSize()
{
    return QSize(m_width, m_height);
}

QList<GLuint> GstDecoder::getTextures()
{
    if(!m_texture){
        m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
        m_texture->setSize(m_width, m_height);
        m_texture->allocateStorage();
    }
    m_gstDecoderThread->fillTexturData(m_texture);
    QList<GLuint> list;
    list << m_texture->textureId();
    return list;
}

GLuint GstDecoder::getEGLTexture()
{
    return m_eglCompositorTexture->getTextureID();
}

void GstDecoder::createGstDecoderThread()
{
    m_gstDecoderThread = new GstDecoderThread(m_width, m_height, m_ip, m_port);
    connect(m_gstDecoderThread, &GstDecoderThread::triggerRender, this, &GstDecoder::onTriggerRender, Qt::QueuedConnection);
}

void GstDecoder::createOffscreenSurface()
{
    m_offscreenSurface = new QOffscreenSurface;
    m_offscreenSurface->setFormat(EGLHelper::context()->format());
    m_offscreenSurface->create();
}

void GstDecoder::createRender()
{
    m_eglCompositorTexture = new EGLCompositorTexture(this, EGLHelper::context());
    connect(m_eglCompositorTexture, &EGLCompositorTexture::render_finish, this, &GstDecoder::onRender_finish);
}

void GstDecoder::onRender_finish()
{
    emit requestUpdate();
}

void GstDecoder::initialize()
{
    createGstDecoderThread();
    createOffscreenSurface();
    createRender();
}

void GstDecoder::onTriggerRender()
{
    m_eglCompositorTexture->render_async();
}

