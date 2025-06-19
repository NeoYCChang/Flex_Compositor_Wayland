#ifndef GSTENCODER_H
#define GSTENCODER_H

#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QDebug>
#include <QOffscreenSurface>
#include <QThread>
#include "gstencoderthread.h"
#include "egl_function/eglhelper.h"
#include "egl_function/eglrender.h"
#include "render_tool/isource.h"
#include "render_tool/compositor.h"

class GstEncoder : public QObject, public iEssentialRenderingTools {
    Q_OBJECT
public:
    explicit GstEncoder(QString name, int id, iSource* source, QRect cropTextureSize, int w, int h, quint16 port, QObject *parent = nullptr);
    ~GstEncoder();
    QSurfaceFormat getFormat() override;
    QSurface* getSurface() override;
    QSize getSize() override;
    EGLHelper::TextureCropSize* getTextureCropSize() override;
    iSource* getSource() override;

public slots:
    void stop();

private:
    int m_width = 960;
    int m_height = 540;
    QMutex mutex;
    iSource* m_source = nullptr;
    EGLRender* m_render = nullptr;
    EGLHelper::TextureCropSize* m_textureCropSize = nullptr;
    QString e_name = "";
    int  e_id = 0;
    quint16 m_port = 50000;
    QOffscreenSurface *m_offscreenSurface = nullptr;
    GstEncoderThread* m_gstEncoderThread = nullptr;

private:
    void createGstEncoderThread();
    void createOffscreenSurface();
    void createRender();
    void render();

private slots:
    void initialize();
};

#endif // GSTENCODER_H
