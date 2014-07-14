#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T14:09:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BatClassify
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    audiodecoder.cpp \
    blob.cpp \
    blob-finder.cpp \
    decision_forest.cpp \
    decision_tree.cpp \
    fft.cpp \
    image.cpp \
    running_stat.cpp \
    STFT.cpp \
    utilities.cpp

HEADERS  += mainwindow.h \
    audiodecoder.h \
    blob.h \
    blob-finder.h \
    data_struct.h \
    decision_forest.h \
    decision_tree.h \
    fft.h \
    image.h \
    running_stat.h \
    STFT.h \
    utilities.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/libsndfile/ -llibsndfile-1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/libsndfile/ -llibsndfile-1
else:unix: LIBS += -L$$PWD/libs/libsndfile/ -llibsndfile-1

INCLUDEPATH += $$PWD/libs/libsndfile
DEPENDPATH += $$PWD/libs/libsndfile

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/libsamplerate/ -llibsamplerate
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/libsamplerate/ -llibsamplerate
else:unix: LIBS += -L$$PWD/libs/libsamplerate/ -llibsamplerate

INCLUDEPATH += $$PWD/libs/libsamplerate
DEPENDPATH += $$PWD/libs/libsamplerate

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/fftw/ -llibfftw3f-3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/fftw/ -llibfftw3f-3
else:unix: LIBS += -L$$PWD/libs/fftw/ -llibfftw3f-3

INCLUDEPATH += $$PWD/libs/fftw
DEPENDPATH += $$PWD/libs/fftw
