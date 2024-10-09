CONFIG += kernel
include(../../config.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = %PROJECT_NAME%

TEMPLATE = app
unix:!macx{
    LIBS_RELATIVE_PATH += \$\$ORIGIN
    CONFIG(debug, debug|release) {
        LIBS_RELATIVE_PATH += \$\$ORIGIN/../lib/debug
        QMAKE_LFLAGS += -Wl,-rpath-link=$$PWD/../lib/debug
    } else {
        LIBS_RELATIVE_PATH += \$\$ORIGIN/../lib/release
        QMAKE_LFLAGS += -Wl,-rpath-link=$$PWD/../lib/release
    }
    MICRO_LIBS_RPATH = $$join(LIBS_RELATIVE_PATH, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${MICRO_LIBS_RPATH}\'
}

SOURCES += main.cpp

DESTDIR = $$BIN_DIR
