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

class GstEncoder : public QObject, public iEssentialRenderingTools {
    Q_OBJECT
public:
    explicit GstEncoder(QString name, int id, iSource* source, EGLHelper::TextureCropSize* textureCropSize, int w, int h, QObject *parent = nullptr);
    ~GstEncoder();
    QSurfaceFormat getFormat() override;
    QSurface* getSurface() override;
    QSize getSize() override;
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
