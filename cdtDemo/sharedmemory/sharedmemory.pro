QT += widgets
DESTDIR = bin

SOURCES += main.cpp \
           dialog.cpp  \
           microdata.cpp \
           sharedmemory.cpp

HEADERS += dialog.h \
    microdata.h \
    private/microdata_p.h \
    private/sharedmemory_p.h \
    sharedmemory.h

# Forms and resources
FORMS += dialog.ui

images.files += $$files($$PWD/*.png)
images.path = $$DESTDIR

COPIES += images
