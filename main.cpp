/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Wayland module
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include "window.h"
#include "compositor.h"
#include "render_tool/displayview.h"
#include "egl_function/eglhelper.h"
#include "codec/gstencoder.h"
#include <QTimer>
#include <QThread>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Compositor compositor("compositor", 17, 1920, 1080);
    //window.show();

    EGLHelper::TextureCropSize* textureCropSize = new  EGLHelper::TextureCropSize{QSize(800, 600), QRect(0,0,800,600)};
    DisplayView display("displayview",0, &compositor, textureCropSize);
    display.resize(800, 600);
    display.show();
    DisplayView display2("displayview",0, &compositor, textureCropSize);
    display2.show();
    DisplayView display3("displayview",0, &compositor, textureCropSize);
    display3.show();
    DisplayView display4("displayview",0, &compositor, textureCropSize);
    display4.show();

    //QImage img = QImage("/home/yuchen/Downloads/long_journey/test.png").scaled(QSize(1920, 1080));
    // qDebug()<<img.size();
    GstEncoder encoder("gstencoder",1, &compositor, textureCropSize, 1920, 1080);
    // QTimer *timer = new QTimer();
    // QObject::connect(timer, &QTimer::timeout, [&encoder, img, &timer]() {
    //     static int count = 0;
    //     if (count < 500) {
    //         encoder.enqueueImage(img);
    //         count++;
    //     }
    //     else{
    //         timer->stop();
    //     }
    // });
    // timer->start(500);  // 每 500 毫秒發送一次

    return app.exec();
}
