TEMPLATE = app 
TARGET = demo6 
HEADERS = mainwindow6.h \
 ../../src/actiongroup.h \
 ../../src/actiongrouplist.h \
 ../../src/qmdiclient.h \
 ../../src/qmdihost.h \
 ../../src/qmdiserver.h \
 ../../src/qmditabwidget.h \
 ../../src/qmdimainwindow.h \
 ../common/helpbrowse.h \
 ../common/qexeditor.h \
 ../../src/qmditabbar.h 
SOURCES = mainwindow6.cpp \
 main6.cpp \
 ../../src/actiongroup.cpp \
 ../../src/actiongrouplist.cpp \
 ../../src/qmdiclient.cpp \
 ../../src/qmdihost.cpp \
 ../../src/qmdiserver.cpp \
 ../../src/qmditabwidget.cpp \
 ../common/helpbrowse.cpp \
 ../common/qexeditor.cpp \
 ../../src/qmditabbar.cpp 
INCLUDEPATH = ../../src/ \
 ../common/ \
 . 
MOC_DIR = ../../tmp/ 
RCC_DIR = ../../tmp/ 
UI_DIR = ../../tmp/ 
OBJECTS_DIR = ../../../tmp/ 
RESOURCES = ../common/common.qrc 
CONFIG += debug_and_release \
 build_all \
 warn_on 
HEADERS -= qmdiworkspace.h
SOURCES -= qmdiworkspace.cpp
SOURCES += ../../src/qmdiworkspace.cpp
HEADERS += ../../src/qmdiworkspace.h
