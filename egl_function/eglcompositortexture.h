#ifndef EGLCOMPOSITORTEXTURE_H
#define EGLCOMPOSITORTEXTURE_H


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
#include <QElapsedTimer>
#include "render_tool/isource.h"

class EGLCompositorTexture: public QObject
{
    Q_OBJECT
public:
    EGLCompositorTexture(iSource* source, QOpenGLContext *share, QObject *parent = nullptr);
    ~EGLCompositorTexture();

    GLuint getTextureID();

public slots:
    void render_async();

signals:
    void render_finish();

private:
    QOpenGLContext *m_context = nullptr;
    iSource* m_source = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;
    QOpenGLVertexArrayObject *m_vao = nullptr;
    QOpenGLFramebufferObject *m_fbo = nullptr;
    bool m_isInit = false;
    QThread *m_thread = nullptr;

private slots:
    void render();
    void destroy_context();

private:
    void init(iSource *source, QOpenGLContext *share);
    void render_init();
    void setNormalMode(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao);
    void createVBO(QOpenGLBuffer*& vbo);
    void createVAO(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao);
    void createFBO(iSource* source);
    void bindFBO();
    void releaseFBO();
};

#endif // EGLCOMPOSITORTEXTURE_H
