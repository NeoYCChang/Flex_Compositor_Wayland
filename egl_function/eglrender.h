#ifndef EGLRENDER_H
#define EGLRENDER_H

#include <QObject>
#include <QOffscreenSurface>
#include <QSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLTextureBlitter>
#include <QOpenGLFramebufferObject>
#include <QThread>
#include "render_tool/iessentialrenderingtools.h"
#include "eglhelper.h"

class EGLRender: public QObject
{
    Q_OBJECT
public:
    EGLRender(iEssentialRenderingTools* renderTool, QOpenGLContext *share, bool isNeedtoImage = false, QObject *parent = nullptr);
    ~EGLRender();

public slots:
    void render_async();

signals:
    void imageReady(const QImage &image);

private:
    QOpenGLContext *m_context = nullptr;
    iEssentialRenderingTools* m_renderTool = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;
    QOpenGLVertexArrayObject *m_vao = nullptr;
    QOpenGLFramebufferObject *m_fbo = nullptr;
    bool m_isNeedtoImage = false;
    bool m_isInit = false;
    QThread *m_thread = nullptr;

private slots:
    void render();
    void destroy_context();

private:
    void init(iEssentialRenderingTools *renderTool, QOpenGLContext *share);
    void render_init();
    void setNormalMode(iEssentialRenderingTools* renderTool, QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao);
    void createVBO(iEssentialRenderingTools* renderTool, QOpenGLBuffer*& vbo);
    void createVAO(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao);
    void createFBO(iEssentialRenderingTools* renderTool);
    void bindFBO();
    void releaseFBO();
    void swapToSurface();
    void notifyImageReady();
};

#endif // EGLRENDER_H
