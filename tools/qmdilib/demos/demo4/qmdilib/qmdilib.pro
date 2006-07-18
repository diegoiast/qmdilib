TEMPLATE = lib
TARGET=qmdi
VERSION=0.0.2
#CONFIG +=   staticlib 
DESTDIR=../


MOC_DIR =  ../../../tmp/
RCC_DIR =  ../../../tmp/
UI_DIR =  ../../../tmp/
OBJECTS_DIR = ../../../tmp/
INCLUDEPATH += ../../../src .



# Input

HEADERS += ../../../src/actiongroup.h \
           ../../../src/actiongrouplist.h \
           ../../../src/qmdiclient.h \
           ../../../src/qmdihost.h \
           ../../../src/qmdiserver.h \
           ../../../src/qmditabwidget.h 

SOURCES += ../../../src/actiongroup.cpp \
           ../../../src/actiongrouplist.cpp \
           ../../../src/qmdiclient.cpp \
           ../../../src/qmdihost.cpp \
           ../../../src/qmdiserver.cpp \
           ../../../src/qmditabwidget.cpp 

