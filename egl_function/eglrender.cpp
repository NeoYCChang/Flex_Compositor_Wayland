#include "eglrender.h"

EGLRender::EGLRender(iEssentialRenderingTools* renderTool, QOpenGLContext *share)
    : m_renderTool(renderTool)
{
    init(m_renderTool, share);
}

EGLRender::~EGLRender()
{
    delete m_context;
    delete m_program;
    if(m_vbo){
        m_vbo->destroy();
    }
    delete m_vbo;
    if(m_vao){
        m_vao->destroy();
    }
    delete m_vao;
}

void EGLRender::render()
{
    if (!m_context->makeCurrent(m_renderTool->getSurface()))
        return;
    m_renderTool->getSource()->startRender();

    QOpenGLFunctions *f = m_context->functions();
    f->glViewport(50, -50, m_renderTool->getSize().width(), m_renderTool->getSize().height());
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glClearColor(.4f, .7f, .1f, 0.5f);

    GLenum currentTarget = GL_TEXTURE_2D;
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_program->bind();
    m_vao->bind();
    const QList<View*> views = m_renderTool->getSource()->views();
    for (View *view : views) {
        QOpenGLTexture * texture = view->getTexture();
        if (!texture)
            continue;
        if (texture->target() != currentTarget) {
            currentTarget = texture->target();
        }
        GLuint textureId = texture->textureId();
        QWaylandSurface *surface = view->surface();
        if (surface && surface->hasContent()) {
            m_program->bind();
            f->glBindTexture(GL_TEXTURE_2D, textureId);
            f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
    m_program->release();
    m_vao->release();
    m_context->swapBuffers(m_renderTool->getSurface());
    m_renderTool->getSource()->endRender();
}

bool EGLRender::init(iEssentialRenderingTools* renderTool, QOpenGLContext *share)
{
    m_context = new QOpenGLContext();
    m_context->setShareContext(share);
    m_context->setFormat(renderTool->getFormat());
    m_context->create();
    if (!m_context->makeCurrent(renderTool->getSurface()))
        return false;
    QOpenGLFunctions *f = m_context->functions();
    f->glClearColor(0.0f, 0.1f, 0.25f, 1.0f);
    f->glViewport(0, 0, renderTool->getSize().width(), renderTool->getSize().height());
    static const char *vertexShaderSource =
        "attribute vec4 a_position;\n"
        "attribute vec2 a_texcoord;\n"
        "varying vec2 v_texcoord;\n"
        "void main()\n"
        "{\n"
        "    v_texcoord = a_texcoord;\n"
        "    gl_Position = a_position;\n"
        "}\n";

    static const char *fragmentShaderSource =
        "uniform sampler2D u_texture;\n"
        "varying vec2 v_texcoord;\n"
        "void main() {\n"
        "   gl_FragColor = texture2D(u_texture, v_texcoord);\n"
        "}\n";
    m_program = new QOpenGLShaderProgram;
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    // Link shader pipeline
    if (!m_program->link())
        return false;

    // Bind shader pipeline for use
    if (!m_program->bind())
        return false;
    m_program->release();
    setNormalMode(m_program, m_vbo, m_vao);

    return true;
}

void EGLRender::setNormalMode(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao)
{
    createVBO(vbo);
    createVAO(program, vbo ,vao);
}

void EGLRender::createVBO(QOpenGLBuffer*& vbo)
{
    if(vbo)
    {
        vbo->destroy();
        delete vbo;
    }
    QVector<QVector3D> vertices = QVector<QVector3D>(4);
    QVector<QVector2D> texcoord = QVector<QVector2D>(4);
    vertices[0] = QVector3D(-1.0f, 1.0f, 0.0f);vertices[1] = QVector3D(-1.0f, -1.0f, 0.0f);
    vertices[2] = QVector3D(1.0f, 1.0f, 0.0f);vertices[3] = QVector3D(1.0f, -1.0f, 0.0f);

    texcoord[0] = QVector2D(0.0f, 0.0f);texcoord[1] = QVector2D(0.0f, 1.0f);
    texcoord[2] = QVector2D(1.0f, 0.0f);texcoord[3] = QVector2D(1.0f, 1.0f);

    vbo = new QOpenGLBuffer;
    vbo->create();
    vbo->bind();
    vbo->allocate(vertices.data(),sizeof(QVector3D)*4 + sizeof(QVector2D)*4);
    vbo->write(sizeof(QVector3D)*4, texcoord.data(), sizeof(QVector2D)*4);

    vbo->release();
}

void EGLRender::createVAO(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao)
{
    if(!program || !vbo)
        return;
    if(vao != nullptr)
    {
        vao->destroy();
        delete vao;
    }
    vbo->bind();
    vao = new QOpenGLVertexArrayObject;
    vao->create();
    vao->bind();
    program->bind();

    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, 0);

    // Offset for texture coordinate
    offset += sizeof(QVector3D) * 4;

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, 0);

    program->release();
    vbo->release();
    vao->release();
}

