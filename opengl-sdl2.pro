TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    application.cpp

LIBS += -lSDL2 -lGL -lGLEW
