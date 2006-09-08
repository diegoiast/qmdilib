# $Id$

TEMPLATE = app
TARGET = demo5
CONFIG += staticlib

DESTDIR 	 = ../
MOC_DIR 	 = ../../../tmp/
RCC_DIR		 = ../../../tmp/
UI_DIR		 = ../../../tmp/
OBJECTS_DIR	 = ../../../tmp/
INCLUDEPATH	+= . ../../../src/ ../../common/

# Input
unix:LIBS	+= -L../ -lqmdi  -lqexedit
win32:LIBS	+= ../libqexedit.a ../libqmdi.a

RESOURCES	+= demo5.qrc
HEADERS		+= mainwindow5.h 
SOURCES		+= mainwindow5.cpp  main5.cpp
