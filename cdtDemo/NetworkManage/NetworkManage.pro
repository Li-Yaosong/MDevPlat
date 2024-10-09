QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    microdata.cpp \
    widget.cpp \
    netutils.cpp

HEADERS += \
    microdata.h \
    private/microdata_p.h \
    private/netutils_p.h \
    widget.h \
    netutils.h

FORMS += \
    widget.ui
