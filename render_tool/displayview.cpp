#include "displayview.h"

DisplayView::DisplayView(QString name, int id, iSource* source, EGLHelper::TextureCropSize* textureCropSize)
    :e_name(name), e_id(id), m_source(source), m_textureCropSize(textureCropSize)
{
    setSurfaceType(QSurface::OpenGLSurface);
    // m_thread = new QThread();
    // this->moveToThread(m_thread);
    // connect(m_thread, &QThread::started, this, &DisplayView::initialize);
    // m_thread->start();
}

DisplayView::~DisplayView()
{
    delete m_render;
}

QSurfaceFormat DisplayView::getFormat()
{
    return this->requestedFormat();
}

QSurface *DisplayView::getSurface()
{
    return this;
}

QSize DisplayView::getSize()
{
    return QSize(this->width(), this->height());
}

iSource *DisplayView::getSource()
{
    return m_source;
}

void DisplayView::initialize()
{
    if (!m_render) {
        m_render = new EGLRender(this, EGLHelper::context());
        if(m_source){
            if (Compositor *compositor = dynamic_cast<Compositor *>(m_source)) {
                connect(compositor, &Compositor::requestUpdate, m_render, &EGLRender::render_async, Qt::QueuedConnection);
            } else {
            }

        }
    }
}

void DisplayView::render()
{
    if (!isExposed()) {
        return;
    }
    if(m_render && m_source){
        m_render->render_async();
    }
}


void DisplayView::exposeEvent(QExposeEvent *e)
{
    QWindow::exposeEvent(e);

    if (!isExposed())
        return;
    initialize();
    render();
}
