include(../../config.pri)

CONFIG += $$MICRO_LIB_DEPENDS

TRANSLATIONS += zh_CN.ts

TRAGETPATH=$$PLUGINS_DIR/$$TARGET

target.path = $$TRAGETPATH
INSTALLS += target

exists($$_PRO_FILE_PWD_/plug.xml) {
    plug.files += $$_PRO_FILE_PWD_/plug.xml
    HAS_XML = 1
}
exists($$_PRO_FILE_PWD_/plug.json) {
    plug.files += $$_PRO_FILE_PWD_/plug.json
    HAS_JSON = 1
}

isEmpty(HAS_XML):isEmpty(HAS_JOSN) {
    error(no plug.xml or plug.json in $$_PRO_FILE_PWD_)
} else {
    plug.path = $$TRAGETPATH
    INSTALLS += plug
}

LANGUAGES = $$files($$_PRO_FILE_PWD_/*.qm)
!isEmpty(LANGUAGES) {
    language.files += $$_PRO_FILE_PWD_/*.qm
    language.path = $$TRAGETPATH
    INSTALLS += language
}

exists($$_PRO_FILE_PWD_/MANIFEST.MF) {
    manifest.files += $$_PRO_FILE_PWD_/MANIFEST.MF
    manifest.path = $$TRAGETPATH
    INSTALLS += manifest
} else {
    error(no MANIFEST.MF in $$_PRO_FILE_PWD_)
}

IMAGES = $$files($$_PRO_FILE_PWD_/images/*)
!isEmpty(IMAGES) {
    images.files += $$_PRO_FILE_PWD_/images/*
    images.path = $$TRAGETPATH/images/
    INSTALLS += images
}

unix:!macx {
    LIBS_RELATIVE_PATH += \$\$ORIGIN
    LIBS_RELATIVE_PATH += \$\$ORIGIN/../..
    MICRO_LIBS_RPATH = $$join(LIBS_RELATIVE_PATH, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${MICRO_LIBS_RPATH}\'
}
