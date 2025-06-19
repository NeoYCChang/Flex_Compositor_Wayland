QT += gui waylandcompositor websockets

LIBS += -L ../../lib

HEADERS += \
    codec/gstdecoder.h \
    codec/gstdecoderthread.h \
    codec/gstencoder.h \
    codec/gstencoderthread.h \
    codec/websocketclient.h \
    codec/websocketserver.h \
    render_tool/compositor.h \
    egl_function/eglcompositortexture.h \
    egl_function/eglhelper.h \
    egl_function/eglrender.h \
    render_tool/displayview.h \
    render_tool/iessentialrenderingtools.h \
    render_tool/isource.h \
    window.h \
    window_controller/extend_display.h

SOURCES += main.cpp \
    codec/gstdecoder.cpp \
    codec/gstdecoderthread.cpp \
    codec/gstencoder.cpp \
    codec/gstencoderthread.cpp \
    codec/websocketclient.cpp \
    codec/websocketserver.cpp \
    render_tool/compositor.cpp \
    egl_function/eglcompositortexture.cpp \
    egl_function/eglhelper.cpp \
    egl_function/eglrender.cpp \
    render_tool/displayview.cpp \
    window.cpp \
    window_controller/extend_display.cpp
INCLUDEPATH += /usr/include/gstreamer-1.0 \
               /usr/include/glib-2.0 \
               /usr/lib/x86_64-linux-gnu/glib-2.0/include

LIBS += -lgstreamer-1.0 -lgstbase-1.0 -lgstapp-1.0 -lglib-2.0 -lgobject-2.0

