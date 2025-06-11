#include "displayview.h"

DisplayView::DisplayView(QString name, int id, Compositor* source, EGLHelper::TextureCropSize* textureCropSize)
    :e_name(name), e_id(id), m_source(source), m_textureCropSize(textureCropSize)
{
    setSurfaceType(QSurface::OpenGLSurface);
    if(m_source){
        connect(m_source, &Compositor::requestUpdate, this, &DisplayView::onRequestUpdate);
    }
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

Compositor *DisplayView::getSource()
{
    return m_source;
}

void DisplayView::render()
{
    if (!isExposed()) {
        return;
    }
    if(m_render && m_source){
        m_render->render();
    }

}

void DisplayView::onRequestUpdate()
{
    render();
}

void DisplayView::exposeEvent(QExposeEvent *e)
{
    QWindow::exposeEvent(e);

    if (!isExposed())
        return;
    if (!m_render && m_source) {
        m_render = new EGLRender(this, EGLHelper::context());
    }
    render();
}
