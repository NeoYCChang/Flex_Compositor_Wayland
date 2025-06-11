QT += gui waylandcompositor

LIBS += -L ../../lib

HEADERS += \
    compositor.h \
    egl_function/eglhelper.h \
    egl_function/eglrender.h \
    render_tool/displayview.h \
    render_tool/iessentialrenderingtools.h \
    window.h \
    window_controller/extend_display.h

SOURCES += main.cpp \
    compositor.cpp \
    egl_function/eglhelper.cpp \
    egl_function/eglrender.cpp \
    render_tool/displayview.cpp \
    window.cpp \
    window_controller/extend_display.cpp

# to make QtWaylandCompositor/... style includes working without installing
INCLUDEPATH += $$PWD/../../include

