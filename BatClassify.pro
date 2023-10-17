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
    utilities.cpp \
    classifieruk.cpp

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
    utilities.h \
    classifieruk.h

FORMS    += mainwindow.ui

##########################   libsndfile
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/libsndfile/ -llibsndfile-1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/libsndfile/ -llibsndfile-1
else:unix: {
    contains(QMAKE_HOST.arch, aarch64):{
        LIBS += /usr/lib/arm-linux-gnueabihf/libsndfile.so
    }else:{
        LIBS += /usr/lib/libsndfile.so
    }
} 
else:macx: LIBS += -L/usr/local/Cellar/libsndfile/1.0.26/lib/ -lsndfile.1

win32:INCLUDEPATH += $$PWD/libs/libsndfile
macx:INCLUDEPATH += /usr/local/Cellar/libsndfile/1.0.26/include

win32:DEPENDPATH += $$PWD/libs/libsndfile
macx:DEPENDPATH += /usr/local/Cellar/libsndfile/1.0.26/include

##########################   libsamplerate
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/libsamplerate/ -llibsamplerate
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/libsamplerate/ -llibsamplerate
else:unix: {
    contains(QMAKE_HOST.arch, aarch64):{
        LIBS += /usr/lib/arm-linux-gnueabihf/libsamplerate.so
    }else{
        LIBS += /usr/lib/libsamplerate.so
    }
}
else:macx: LIBS += -L/usr/local/Cellar/libsamplerate/0.1.8/lib/ -lsamplerate

win32:INCLUDEPATH += $$PWD/libs/libsamplerate
macx:INCLUDEPATH += /usr/local/Cellar/libsamplerate/0.1.8/include

win32:DEPENDPATH += $$PWD/libs/libsamplerate
macx:DEPENDPATH += /usr/local/Cellar/libsamplerate/0.1.8/include

##########################   FFTW
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/fftw/ -llibfftw3f-3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/fftw/ -llibfftw3f-3
else:unix: {
    contains(QMAKE_HOST.arch, aarch64):{
        LIBS += /usr/lib/arm-linux-gnueabihf/libfftw3f.so
    }else{
        LIBS += /usr/lib/libfftw3f.so
    }
}
else:mac: LIBS += -L/usr/local/Cellar/fftw/3.3.4_1/lib -lfftw3f.3

win32:INCLUDEPATH += $$PWD/libs/fftw
macx:INCLUDEPATH += /usr/local/Cellar/fftw/3.3.4_1/include
win32:DEPENDPATH += $$PWD/libs/fftw
macx:DEPENDPATH += /usr/local/Cellar/fftw/3.3.4_1/include
