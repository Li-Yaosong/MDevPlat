load(iss)

isEmpty(BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    BUILD_TREE = $$clean_path($$OUT_PWD)
    BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}

BIN_DIR         = $$BUILD_TREE/bin
LIB_DIR         = $$BUILD_TREE/lib

SRC_DIR         = $$PWD/src
LANGUAGE_DIR    = $$BIN_DIR/translations
LIB_SRC_DIR     = $$SRC_DIR/libs
PLUGIN_SRC_DIR  = $$SRC_DIR/plugins
PLUGINS_DIR     = $$BIN_DIR/plugins
INCLUDE_DIR     = $$BUILD_TREE/include

LIBS += -L$$LIB_DIR
