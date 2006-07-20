TEMPLATE = app
TARGET = mdi-tab
DEPENDPATH += .
INCLUDEPATH += . ../../src/
DESTDIR = ./
MOC_DIR =  ../../tmp/
RCC_DIR =  ../../tmp/
UI_DIR =  ../../tmp/
OBJECTS_DIR = ../../tmp/


# Input
RESOURCES += mdi-tab.qrc

HEADERS += ../../src/actiongroup.h \
           ../../src/actiongrouplist.h \
           ../../src/qmdiclient.h \
           ../../src/qmdihost.h \
           ../../src/qmdiserver.h \
           ../../src/qmditabwidget.h \
           qexeditor.h \
           helpbrowse.h \
           mainwindow2.h \

SOURCES += ../../src/actiongroup.cpp \
           ../../src/actiongrouplist.cpp \
           ../../src/qmdiclient.cpp \
           ../../src/qmdihost.cpp \
           ../../src/qmdiserver.cpp \
           ../../src/qmditabwidget.cpp \
           qexeditor.cpp \
           mainwindow2.cpp \
           helpbrowse.cpp \
           main2.cpp \

