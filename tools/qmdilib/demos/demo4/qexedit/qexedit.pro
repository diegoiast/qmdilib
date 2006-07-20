TEMPLATE = lib
TARGET=qexedit
VERSION=0.0.2
#CONFIG += staticlib
DESTDIR=../

MOC_DIR =  ../../../tmp/
RCC_DIR =  ../../../tmp/
UI_DIR =  ../../../tmp/
OBJECTS_DIR = ../../../tmp/
INCLUDEPATH += ../../../src .



# Input
unix:LIBS += -lqmdi -L../
win32:LIBS += -lqmdi0 -L../

HEADERS += ../../demo2/qexeditor.h \
           ../../demo2/helpbrowse.h 

SOURCES += ../../demo2/qexeditor.cpp \
           ../../demo2/helpbrowse.cpp 

