#ifndef IESSENTIALRENDERINGTOOLS_H
#define IESSENTIALRENDERINGTOOLS_H
#include <QSurface>
#include <QSurfaceFormat>
#include <QSize>
#include <QList>
#include <QOpenGLContext>
#include "isource.h"

class iEssentialRenderingTools
{
public:
    virtual ~iEssentialRenderingTools() = default;

    virtual QSurfaceFormat getFormat() = 0;
    virtual QSurface* getSurface() = 0;
    virtual QSize getSize() = 0;
    virtual iSource* getSource() = 0;
};

#endif // IESSENTIALRENDERINGTOOLS_H
