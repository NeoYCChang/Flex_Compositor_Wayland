#ifndef ISOURCE_H
#define ISOURCE_H
#include <QSurface>
#include <QSurfaceFormat>
#include <QSize>
#include <QList>
#include <QOpenGLContext>

class iSource
{
public:
    virtual ~iSource() = default;

    virtual QSurfaceFormat getFormat() = 0;
    virtual QSurface* getSurface() = 0;
    virtual QSize getSize() = 0;
    virtual QList<GLuint> getTextures() = 0;
    virtual GLuint getEGLTexture() = 0;
};
#endif // ISOURCE_H
