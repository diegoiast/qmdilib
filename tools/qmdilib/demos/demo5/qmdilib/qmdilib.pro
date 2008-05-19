# $Id$

TEMPLATE = lib
TARGET = qmdi
CONFIG += staticlib 
VERSION = 0.0.5

DESTDIR		 = ../
MOC_DIR		 = ../../../tmp/
RCC_DIR		 = ../../../tmp/
UI_DIR		 = ../../../tmp/
OBJECTS_DIR 	 = ../../../tmp/
INCLUDEPATH	+= . ../../../src/ ../../common/

# Input
HEADERS += ../../../src/qmdiactiongroup.h \
           ../../../src/qmdiactiongrouplist.h \
           ../../../src/qmdiclient.h \
           ../../../src/qmdihost.h \
           ../../../src/qmdiserver.h \
           ../../../src/qmditabwidget.h 

SOURCES += ../../../src/qmdiactiongroup.cpp \
           ../../../src/qmdiactiongrouplist.cpp \
           ../../../src/qmdiclient.cpp \
           ../../../src/qmdihost.cpp \
           ../../../src/qmdiserver.cpp \
           ../../../src/qmditabwidget.cpp 

