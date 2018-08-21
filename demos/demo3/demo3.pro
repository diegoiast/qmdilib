# $Id$
include ( ../common/demos.pri )

DESTDIR  = ../../
TEMPLATE = app
CONFIG  += silent 
TARGET   = demo3
HEADERS += mainwindow3.h 
SOURCES += mainwindow3.cpp  main3.cpp

# link against qmdilib
INCLUDEPATH += ../../src/
LIBS += -L../../ -lqmdi
#this magic makes the search of the *.so in the same dir
unix:{
	QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,.\'
}
