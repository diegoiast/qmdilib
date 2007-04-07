# $Id$

TEMPLATE = lib
TARGET = qmdi
CONFIG += staticlib 
VERSION = 0.0.4

DESTDIR		 = ../
MOC_DIR		 = ../../../tmp/
RCC_DIR		 = ../../../tmp/
UI_DIR		 = ../../../tmp/
OBJECTS_DIR 	 = ../../../tmp/
INCLUDEPATH	+= . ../../../src/ ../../common/

# Input
HEADERS += ../../../src/actiongroup.h \
           ../../../src/actiongrouplist.h \
           ../../../src/qmdiclient.h \
           ../../../src/qmdihost.h \
           ../../../src/qmdiserver.h \
           ../../../src/qmditabbar.h \
           ../../../src/qmditabwidget.h 

SOURCES += ../../../src/actiongroup.cpp \
           ../../../src/actiongrouplist.cpp \
           ../../../src/qmdiclient.cpp \
           ../../../src/qmdihost.cpp \
           ../../../src/qmdiserver.cpp \
           ../../../src/qmditabbar.cpp \
           ../../../src/qmditabwidget.cpp 

