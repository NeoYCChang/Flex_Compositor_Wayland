#include "eglhelper.h"

// QOpenGLContext* EGLHelper::m_context = []() {
//     QOpenGLContext* ctx = new QOpenGLContext();
//     QSurfaceFormat format;
//     format.setDepthBufferSize(24);
//     format.setStencilBufferSize(8);
//     ctx->setFormat(format);

//     // if (!ctx->create()) {
//     //     qWarning() << "Failed to create OpenGL context";
//     // }
//     return ctx;
// }();
QOpenGLContext* EGLHelper::m_context = nullptr;

EGLHelper::EGLHelper() {}

QOpenGLContext *EGLHelper::context()
{
    if(!m_context){
        m_context = new QOpenGLContext();
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        m_context->setFormat(format);
        if (!m_context->create()) {
            qWarning() << "Failed to create OpenGL context";
            return nullptr;
        }
        return m_context;
    }
    else if(m_context->isValid()){
        return m_context;
    }
    else{
        return nullptr;
    }
}
