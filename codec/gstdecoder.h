#ifndef GSTDECODER_H
#define GSTDECODER_H

#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLTexture>
#include "render_tool/isource.h"
#include "egl_function/eglcompositortexture.h"
#include "egl_function/eglhelper.h"
#include "gstdecoderthread.h"

class GstDecoder : public QObject, public iSource
{
    Q_OBJECT
public:
    explicit GstDecoder(QString name, int id,  int w, int h, QString ip, quint16 port);
    ~GstDecoder();

signals:
    void requestUpdate();

//iSource
public:
    QSurfaceFormat getFormat() override;
    QSurface* getSurface() override;
    QSize getSize() override;
    QList<GLuint> getTextures() override;
    GLuint getEGLTexture() override;

private:
    int m_width = 960;
    int m_height = 540;
    QString e_name = "";
    int  e_id = 0;
    EGLCompositorTexture* m_eglCompositorTexture = nullptr;
    QOffscreenSurface *m_offscreenSurface = nullptr;
    GstDecoderThread* m_gstDecoderThread = nullptr;
    QString m_ip  = "127.0.0.1";
    quint16 m_port = 50000;
    QOpenGLTexture *m_texture = nullptr;

private:
    void createGstDecoderThread();
    void createOffscreenSurface();
    void createRender();

private slots:
    void onRender_finish();
    void initialize();
    void onTriggerRender();
};

#endif // GSTDECODER_H
