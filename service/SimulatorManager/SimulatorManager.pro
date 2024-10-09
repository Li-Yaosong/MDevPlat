TEMPLATE = subdirs
CONFIG += order

SUBDIRS += botan \
    app


botan.file = botan/botan.pro
app.file = app/app.pro
