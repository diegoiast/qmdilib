TEMPLATE = lib
TARGET = qexedit
CONFIG += staticlib
VERSION = 0.0.2

DESTDIR = ../
MOC_DIR =  ../../../tmp/
RCC_DIR =  ../../../tmp/
UI_DIR =  ../../../tmp/
OBJECTS_DIR = ../../../tmp/
INCLUDEPATH += . ../../../src

# Input
unix:LIBS += -L../ -lqmdi 

HEADERS += ../../demo2/qexeditor.h \
           ../../demo2/helpbrowse.h 

SOURCES += ../../demo2/qexeditor.cpp \
           ../../demo2/helpbrowse.cpp 
