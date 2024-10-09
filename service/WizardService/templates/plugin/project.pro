QT       += core

CONFIG += kernel

greaterThan(QT_MAJOR_VERSION, 4): QT += 
lessThan(QT_MAJOR_VERSION, 5): QT += qt3support

TARGET = %PluginName%
TEMPLATE = %TEMPLATETYPE%

include(../plugins.pri)

%DEFINESSHARDLIBRARY%

SOURCES += %SOURCES%

HEADERS += %HEADERS%
%FORM%
%ExeSupport%
%INSTALLS_INTERFACE_FILES%
%INSTALLS_PRF_FILE%
OTHER_FILES += MANIFEST.MF \
    plug.xml



