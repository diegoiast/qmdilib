# $Id$
include ( ../common/demos.pri )

DESTDIR  = ../../
TEMPLATE = app
CONFIG  += silent 
TARGET   = demo2
HEADERS += mainwindow2.h 
SOURCES += mainwindow2.cpp  main2.cpp

# link against qmdilib
INCLUDEPATH += ../../src/
LIBS += ../../libqmdi.a
