#ifndef EGLRENDER_H
#define EGLRENDER_H
#include <QOffscreenSurface>
#include <QSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLTextureBlitter>
#include "render_tool/iessentialrenderingtools.h"
#include "compositor.h"

class EGLRender
{
public:
    EGLRender(iEssentialRenderingTools* renderTool, QOpenGLContext *share);
    ~EGLRender();
    void render();

private:
    QOpenGLContext *m_context = nullptr;
    iEssentialRenderingTools* m_renderTool = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;
    QOpenGLVertexArrayObject *m_vao = nullptr;

private:
    bool init(iEssentialRenderingTools *renderTool, QOpenGLContext *share);
    void setNormalMode(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao);
    void createVBO(QOpenGLBuffer*& vbo);
    void createVAO(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao);
};

#endif // EGLRENDER_H
