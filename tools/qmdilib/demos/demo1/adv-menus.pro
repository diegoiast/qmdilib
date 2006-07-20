TEMPLATE = app
TARGET += 
DEPENDPATH += .
INCLUDEPATH += . ../../src/
DESTDIR = ./
MOC_DIR =  ../../tmp/
RCC_DIR =  ../../tmp/
UI_DIR =  ../../tmp/
OBJECTS_DIR = ../../tmp/


# Input
RESOURCES += adv-menus.qrc

HEADERS += ../../src/actiongroup.h \
           ../../src/actiongrouplist.h \
           ../../src/qmdiclient.h \
           ../../src/qmdihost.h \
           ../../src/qmdiserver.h \
           mainwindow.h \
	   	   
SOURCES += ../../src/actiongroup.cpp \
           ../../src/actiongrouplist.cpp \
           ../../src/qmdiclient.cpp \
           ../../src/qmdihost.cpp \
           ../../src/qmdiserver.cpp \
           mainwindow.cpp \
           main.cpp \
