# $Id$
QMDILIB_SRC_DIR  = ../../src/
INCLUDEPATH += $$QMDILIB_SRC_DIR
include ($$QMDILIB_SRC_DIR/qmdilib.pri)

DESTDIR  = ../../
TEMPLATE = app
CONFIG  += silent 
TARGET   = demo1
HEADERS += mainwindow.h 
SOURCES += mainwindow.cpp  main.cpp 

