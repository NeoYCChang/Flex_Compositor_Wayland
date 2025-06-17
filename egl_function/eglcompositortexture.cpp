#include "eglcompositortexture.h"

EGLCompositorTexture::EGLCompositorTexture(iSource* source, QOpenGLContext *share, QObject *parent)
    : QObject(parent), m_source(source)
{
    init(m_source, share);
}

EGLCompositorTexture::~EGLCompositorTexture()
{
    QMetaObject::invokeMethod(this, "destroy_context", Qt::QueuedConnection);
    if(m_thread){
        m_thread->requestInterruption();
        m_thread->quit();         // Asks event loop to exit (if you use one)
        m_thread->wait();         // Blocks until thread exits
    }
    delete m_thread;
}

GLuint EGLCompositorTexture::getTextureID()
{
    m_fbo->texture();
}

void EGLCompositorTexture::render_async()
{
    if(m_isInit){
        QMetaObject::invokeMethod(this, "render", Qt::QueuedConnection);
    }
}

void EGLCompositorTexture::render()
{
    // QElapsedTimer timer;
    // timer.start();  // ⏱ 開始計時
    if (!m_context->makeCurrent(m_source->getSurface()))
        return;
    QOpenGLFunctions *f = m_context->functions();
    f->glViewport(0, 0, m_source->getSize().width(), m_source->getSize().height());
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glClearColor(.4f, .7f, .1f, 0.5f);

    GLenum currentTarget = GL_TEXTURE_2D;
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bindFBO();
    m_program->bind();
    m_vao->bind();
    QList<GLuint> textureids = m_source->getTextures();
    m_program->bind();
    for (GLuint textureid : textureids) {
        glBindTexture(GL_TEXTURE_2D, textureid);
        f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    m_program->release();
    m_vao->release();
    releaseFBO();
    f->glFinish();
    emit render_finish();
    // qDebug() << "enqueueImage elapsed:" << timer.nsecsElapsed() / 1e6 << "ms";  // 顯示毫秒（浮點數）
}

void EGLCompositorTexture::init(iSource* source, QOpenGLContext *share)
{
    m_context = new QOpenGLContext();
    m_context->setShareContext(share);
    m_context->setFormat(source->getFormat());
    m_context->create();

    m_thread = new QThread();
    m_context->moveToThread(m_thread);
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, [=]() {
        if (!m_context->makeCurrent(source->getSurface()))
            return;
        QOpenGLFunctions *f = m_context->functions();
        f->glClearColor(0.0f, 0.1f, 0.25f, 1.0f);
        f->glViewport(0, 0, source->getSize().width(), source->getSize().height());
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
            return;

        // Bind shader pipeline for use
        if (!m_program->bind())
            return;
        m_program->release();
        setNormalMode(m_program, m_vbo, m_vao);
        createFBO(source);
        m_isInit = true;
        render_init();
    });

    m_thread->start();
}

void EGLCompositorTexture::render_init()
{
    if (!m_context->makeCurrent(m_source->getSurface()))
        return;

    QOpenGLFunctions *f = m_context->functions();
    f->glViewport(0, 0, m_source->getSize().width(), m_source->getSize().height());
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glClearColor(.4f, .7f, .1f, 0.5f);

    GLenum currentTarget = GL_TEXTURE_2D;
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bindFBO();
    m_program->bind();
    m_vao->bind();
    QList<GLuint> textureids = m_source->getTextures();
    m_program->bind();
    for (GLuint textureid : textureids) {
        glBindTexture(GL_TEXTURE_2D, textureid);
        f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    m_program->release();
    m_vao->release();
    releaseFBO();
}

void EGLCompositorTexture::setNormalMode(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao)
{
    createVBO(vbo);
    createVAO(program, vbo ,vao);
}

void EGLCompositorTexture::createVBO(QOpenGLBuffer*& vbo)
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

void EGLCompositorTexture::createVAO(QOpenGLShaderProgram*& program, QOpenGLBuffer*& vbo, QOpenGLVertexArrayObject*& vao)
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

void EGLCompositorTexture::createFBO(iSource* source)
{
    m_fbo = new QOpenGLFramebufferObject(source->getSize(), QOpenGLFramebufferObject::Depth);
}

void EGLCompositorTexture::bindFBO()
{
    if(m_fbo){
        m_fbo->bind();
    }
}

void EGLCompositorTexture::releaseFBO()
{
    if(m_fbo){
        m_fbo->release();
    }
}

void EGLCompositorTexture::destroy_context()
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
    delete m_fbo;
}



