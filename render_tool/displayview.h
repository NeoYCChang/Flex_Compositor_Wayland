#ifndef DISPLAYVIEW_H
#define DISPLAYVIEW_H

#include <QObject>
#include <QWindow>
#include <QSize>
#include <QRect>
#include "iessentialrenderingtools.h"
#include "egl_function/eglhelper.h"
#include "egl_function/eglrender.h"
#include "isource.h"

class DisplayView : public QWindow, public iEssentialRenderingTools
{
    Q_OBJECT
public:
    DisplayView(QString name, int id, iSource* source, QRect cropTextureSize);
    ~DisplayView();
    QSurfaceFormat getFormat() override;
    QSurface* getSurface() override;
    QSize getSize() override;
    EGLHelper::TextureCropSize* getTextureCropSize() override;
    iSource* getSource() override;

private:
    iSource* m_source = nullptr;
    EGLRender* m_render = nullptr;
    EGLHelper::TextureCropSize* m_textureCropSize = nullptr;
    QString e_name = "";
    int  e_id = 0;


private:
    void initialize();
    void render();

protected:
    void exposeEvent(QExposeEvent *e) override;
};

#endif // DISPLAYVIEW_H
