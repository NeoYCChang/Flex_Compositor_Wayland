/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Wayland module
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "compositor.h"
#include "window.h"

#include <QtWaylandCompositor/QWaylandOutput>
#include <QtWaylandCompositor/QWaylandIviApplication>
#include <QtWaylandCompositor/QWaylandIviSurface>
#include <QtWaylandCompositor/QWaylandSeat>

#include <QRandomGenerator>
#include <QOpenGLFunctions>
#include "egl_function/eglcompositortexture.h"

QOpenGLTexture *View::getTexture() {
    if (advance())
        m_texture = currentBuffer().toOpenGLTexture();
    return m_texture;
}

QPoint View::mapToLocal(const QPoint &globalPos) const
{
    return globalPos - globalPosition();
}


// Normally, an IVI based compositor would have a design where each window has
// a defined position, based on the id. In this example, we just assign a random position.

void View::initPosition(const QSize &screenSize, const QSize &surfaceSize)
{
    if (m_positionSet)
        return;
    QRandomGenerator rand(0);
    int xrange = qMax(screenSize.width() - surfaceSize.width(), 1);
    int yrange = qMax(screenSize.height() - surfaceSize.height(), 1);
    setGlobalPosition(QPoint(rand.bounded(xrange), rand.bounded(yrange)));
}

Compositor::Compositor(QString name, int id,  int w, int h)
    : e_name(name), e_id(id), m_width(w), m_height(h)
{
    create();
    //connect(window, &Window::glReady, this, [this] { create(); });
}

Compositor::~Compositor()
{
    delete m_window;
    delete m_iviApplication;
    delete m_xdgShell;
    delete m_wlShell;
    for (View* view : m_views) {
        delete view;
    }
    m_views.clear();
    delete m_eglCompositorTexture;
    delete m_offscreenSurface;
}

void Compositor::create()
{
    m_window = new QWindow();
    m_window->resize(m_width, m_height);
    QWaylandOutput *output = new QWaylandOutput(this, m_window);
    QWaylandOutputMode mode(m_window->size(), 60000);
    output->addMode(mode, true);
    output->setCurrentMode(mode);
    QWaylandCompositor::create();
    createOffscreenSurface();

    m_iviApplication = new QWaylandIviApplication(this);
    connect(m_iviApplication, &QWaylandIviApplication::iviSurfaceCreated, this, &Compositor::onIviSurfaceCreated);
    m_xdgShell = new QWaylandXdgShell(this);
    connect(m_xdgShell, &QWaylandXdgShell::toplevelCreated, this, &Compositor::onToplevelCreated);
    m_wlShell = new QWaylandWlShell(this);
    connect(m_wlShell, &QWaylandWlShell::wlShellSurfaceCreated, this, &Compositor::onWlShellSurfaceCreated);
}

View *Compositor::viewAt(const QPoint &position)
{
    // Since views are stored in painting order (back to front), we have to iterate backwards
    // to find the topmost view at a given point.
    for (auto it = m_views.crbegin(); it != m_views.crend(); ++it) {
        View *view = *it;
        if (view->globalGeometry().contains(position))
            return view;
    }
    return nullptr;
}

void Compositor::raise(View *view)
{
    m_views.removeAll(view);
    m_views.append(view);
    defaultSeat()->setKeyboardFocus(view->surface());
    triggerRender();
}

static inline QPoint mapToView(const View *view, const QPoint &position)
{
    return view ? view->mapToLocal(position) : position;
}

void Compositor::handleMousePress(const QPoint &position, Qt::MouseButton button)
{
    if (!m_mouseView) {
        if (m_mouseView = viewAt(position))
            raise(m_mouseView);
    }
    auto *seat = defaultSeat();
    seat->sendMouseMoveEvent(m_mouseView, mapToView(m_mouseView, position));
    seat->sendMousePressEvent(button);
}

void Compositor::handleMouseRelease(const QPoint &position, Qt::MouseButton button, Qt::MouseButtons buttons)
{
    auto *seat = defaultSeat();
    seat->sendMouseMoveEvent(m_mouseView, mapToView(m_mouseView, position));
    seat->sendMouseReleaseEvent(button);

    if (buttons == Qt::NoButton) {
        View *newView = viewAt(position);
        if (newView != m_mouseView)
            seat->sendMouseMoveEvent(newView, mapToView(newView, position));
        m_mouseView = nullptr;
    }
}

void Compositor::handleMouseMove(const QPoint &position)
{
    View *view = m_mouseView ? m_mouseView.data() : viewAt(position);
    defaultSeat()->sendMouseMoveEvent(view, mapToView(view, position));
}

void Compositor::handleMouseWheel(const QPoint &angleDelta)
{
    // TODO: fix this to send a single event, when diagonal scrolling is supported
    if (angleDelta.x() != 0)
        defaultSeat()->sendMouseWheelEvent(Qt::Horizontal, angleDelta.x());
    if (angleDelta.y() != 0)
        defaultSeat()->sendMouseWheelEvent(Qt::Vertical, angleDelta.y());
}

void Compositor::handleKeyPress(quint32 nativeScanCode)
{
    defaultSeat()->sendKeyPressEvent(nativeScanCode);
}

void Compositor::handleKeyRelease(quint32 nativeScanCode)
{
    defaultSeat()->sendKeyReleaseEvent(nativeScanCode);
}

QSurfaceFormat Compositor::getFormat()
{
    return m_offscreenSurface->requestedFormat();
}

QSurface *Compositor::getSurface()
{
    return m_offscreenSurface;
}

QSize Compositor::getSize()
{
    return QSize(m_width, m_height);
}

QList<GLuint> Compositor::getTextures()
{
    QList<GLuint> list;
    for (auto it = m_views.crbegin(); it != m_views.crend(); ++it) {
        View *view = *it;
        QOpenGLTexture * texture = view->getTexture();
        if(texture){
            list << texture->textureId();
        }
    }
    return list;
}

GLuint Compositor::getEGLTexture()
{
    return m_eglCompositorTexture->getTextureID();
}


void Compositor::onIviSurfaceCreated(QWaylandIviSurface *iviSurface)
{
    View *view = new View(QString::number(iviSurface->iviId()));
    view->setSurface(iviSurface->surface());
    view->setOutput(outputFor(m_window));

    m_views << view;
    connect(view, &QWaylandView::surfaceDestroyed, this, &Compositor::viewSurfaceDestroyed);
    connect(iviSurface->surface(), &QWaylandSurface::redraw, this, &Compositor::triggerRender);
}


void Compositor::onToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface)
{
    View *view = new View(toplevel->appId());
    view->setSurface(xdgSurface->surface());
    view->setOutput(outputFor(m_window));

    m_views << view;
    connect(view, &QWaylandView::surfaceDestroyed, this, &Compositor::viewSurfaceDestroyed);
    connect(xdgSurface->surface(), &QWaylandSurface::redraw, this, &Compositor::triggerRender);
}

void Compositor::onWlShellSurfaceCreated(QWaylandWlShellSurface *shellSurface)
{
    View *view = new View(shellSurface->title());
    view->setSurface(shellSurface->surface());
    view->setOutput(outputFor(m_window));

    m_views << view;
    connect(view, &QWaylandView::surfaceDestroyed, this, &Compositor::viewSurfaceDestroyed);
    connect(shellSurface->surface(), &QWaylandSurface::redraw, this, &Compositor::triggerRender);
}

void Compositor::onSurfaceDestroyed()
{
    triggerRender();
}

void Compositor::viewSurfaceDestroyed()
{
    View *view = qobject_cast<View*>(sender());
    m_views.removeAll(view);
    delete view;
    triggerRender();
}

void Compositor::triggerRender()
{
    //m_window->requestUpdate();
    startRender();
}

void Compositor::startRender()
{
    QWaylandOutput *out = defaultOutput();
    if (out)
        out->frameStarted();
    if(m_eglCompositorTexture)
        m_eglCompositorTexture->render_async();
}


void Compositor::onRender_finish()
{
    emit requestUpdate();
    endRender();
}


void Compositor::endRender()
{
    QWaylandOutput *out = defaultOutput();
    if (out)
        out->sendFrameCallbacks();
}

void Compositor::createOffscreenSurface()
{
    m_offscreenSurface = new QOffscreenSurface;
    m_offscreenSurface->setFormat(EGLHelper::context()->format());
    m_offscreenSurface->create();
    m_eglCompositorTexture = new EGLCompositorTexture(this, EGLHelper::context());
    connect(m_eglCompositorTexture, &EGLCompositorTexture::render_finish, this, &Compositor::onRender_finish);
}
