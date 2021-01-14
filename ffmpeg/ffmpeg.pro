include(../libs.pri)
include(../ffmpeg.pri)

QT += widgets multimedia

DEFINES += FFMPEG_LIBRARY
TARGET = $$replaceLibName(ffmpeg)

SOURCES += \
    avaudio.cpp \
    avcontextinfo.cpp \
    avimage.cpp \
    codeccontext.cpp \
    formatcontext.cpp \
    packet.cpp \
    player.cpp \
    playframe.cpp

HEADERS += \
    avaudio.h \
    avcontextinfo.h \
    avimage.h \
    codeccontext.h \
    ffmepg_global.h \
    formatcontext.h \
    packet.h \
    player.h \
    playframe.h
