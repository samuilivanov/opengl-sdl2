TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

SOURCES += \
    src/application.cpp \
    src/renderer.cpp \
    src/vertexbuffer.cpp \
    src/indexbuffer.cpp \
    src/vertexarray.cpp \
    src/vertexbufferlayout.cpp \
    src/shader.cpp

LIBS += -lSDL2 -lGL -lGLEW

DISTFILES += \
    res/shaders/basic.shader

HEADERS += \
    src/renderer.h \
    src/vertexbuffer.h \
    src/indexbuffer.h \
    src/vertexarray.h \
    src/vertexbufferlayout.h \
    src/shader.h
