QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Chord-Detector-and-Chromagram/src/ChordDetector.cpp \
    ../Chord-Detector-and-Chromagram/src/Chromagram.cpp \
        audiobackend.cpp \
        main.cpp \



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../Chord-Detector-and-Chromagram/src/ChordDetector.h \
    ../Chord-Detector-and-Chromagram/src/Chromagram.h \
    VERSION.h \
    audiobackend.h \



unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += jack

DEFINES += USE_FFTW
unix: PKGCONFIG += fftw3
