# $Id$

TEMPLATE = lib
TARGET=qexedit
VERSION=0.0.3
#CONFIG += staticlib
DESTDIR=../

MOC_DIR =  ../../../tmp/
RCC_DIR =  ../../../tmp/
UI_DIR =  ../../../tmp/
OBJECTS_DIR = ../../../tmp/
INCLUDEPATH += ../../../src ../../common .



# Input
#unix:LIBS += -lqmdi -L../
#win32:LIBS += -lqmdi0 -L../

HEADERS += ../../common/qexeditor.h \
           ../../common/helpbrowse.h 

SOURCES += ../../common/qexeditor.cpp \
           ../../common/helpbrowse.cpp 

