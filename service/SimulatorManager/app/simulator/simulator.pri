INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/simulator.h \
    $$PWD/globalsimulatormanager.h \
    $$PWD/abstractsimulatormanager.h \
    $$PWD/androidsimulatormanager.h \
    $$PWD/linuxsimulatormanager.h \
    $$PWD/androidsimulator.h \
    $$PWD/linuxsimulator.h \
    $$PWD/androidsettings.h

SOURCES += \
    $$PWD/simulator.cpp \
    $$PWD/globalsimulatormanager.cpp \
    $$PWD/abstractsimulatormanager.cpp \
    $$PWD/androidsimulatormanager.cpp \
    $$PWD/linuxsimulatormanager.cpp \
    $$PWD/androidsimulator.cpp \
    $$PWD/linuxsimulator.cpp \
    $$PWD/androidsettings.cpp


#include($$PWD/filebrowser/filebrowser.pri)
