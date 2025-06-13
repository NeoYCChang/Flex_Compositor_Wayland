#ifndef DISPLAYVIEW_H
#define DISPLAYVIEW_H

#include <QObject>
#include <QWindow>
#include <QSize>
#include "iessentialrenderingtools.h"
#include "egl_function/eglhelper.h"
#include "egl_function/eglrender.h"
#include "compositor.h"

class DisplayView : public QWindow, public iEssentialRenderingTools
{
    Q_OBJECT
public:
    DisplayView(QString name, int id, Compositor* source, EGLHelper::TextureCropSize* textureCropSize);
    ~DisplayView();
    QSurfaceFormat getFormat() override;
    QSurface* getSurface() override;
    QSize getSize() override;
    Compositor* getSource() override;

private:
    Compositor* m_source = nullptr;
    EGLRender* m_render = nullptr;
    EGLHelper::TextureCropSize* m_textureCropSize = nullptr;
    QString e_name = "";
    int  e_id = 0;


private:
    void initialize();
    void render();

private slots:
    void onRequestUpdate();

protected:
    void exposeEvent(QExposeEvent *e) override;
};

#endif // DISPLAYVIEW_H
