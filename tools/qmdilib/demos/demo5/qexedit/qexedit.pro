# $Id$

TEMPLATE = lib
TARGET = qexedit
CONFIG += staticlib
VERSION = 0.0.3

DESTDIR		 = ../
MOC_DIR		 = ../../../tmp/
RCC_DIR		 = ../../../tmp/
UI_DIR		 = ../../../tmp/ 
OBJECTS_DIR	 = ../../../tmp/
INCLUDEPATH	+= . ../../../src ../../common

# Input
LIBS += -L../ -lqmdi 

HEADERS += ../../common/qexeditor.h \
           ../../common/helpbrowse.h 

SOURCES += ../../common/qexeditor.cpp \
           ../../common/helpbrowse.cpp 
