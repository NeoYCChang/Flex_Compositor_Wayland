#ifndef IESSENTIALRENDERINGTOOLS_H
#define IESSENTIALRENDERINGTOOLS_H
#include <QSurface>
#include <QSurfaceFormat>
#include <QSize>
#include "compositor.h"

class iEssentialRenderingTools
{
public:
    virtual ~iEssentialRenderingTools() = default;

    virtual QSurfaceFormat getFormat() = 0;
    virtual QSurface* getSurface() = 0;
    virtual QSize getSize() = 0;
    virtual Compositor* getSource() = 0;
};

#endif // IESSENTIALRENDERINGTOOLS_H
