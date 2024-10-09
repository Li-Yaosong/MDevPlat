QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle
QMAKE_LFLAGS += -no-pie
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += USE_CMAKE_WIZARD
DEFINES += USE_QMAKE_WIZARD

SOURCES += \
        clientworker.cpp \
        main.cpp \
        server.cpp \
        tcpdatareceiver.cpp \
        wizardutils.cpp

HEADERS += \
    clientworker.h \
    server.h \
    tcpdatareceiver.h \
    wizarddefines.h \
    wizardutils.h

unix:DESTDIR = $(HOME)

temlate.files += $$files($$PWD/templates)
temlate.path = $$DESTDIR

COPIES += temlate
