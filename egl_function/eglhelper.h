#ifndef EGLHELPER_H
#define EGLHELPER_H

#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QSize>
#include <QRect>


class EGLHelper
{
public:
    EGLHelper();
    struct TextureCropSize {
        QSize textureSize;
        QRect crop;
    };
    static QOpenGLContext *context();

private:
    static QOpenGLContext *m_context;
};

#endif // EGLHELPER_H
