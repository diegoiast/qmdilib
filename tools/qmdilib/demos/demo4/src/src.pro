TEMPLATE = app
TARGET = demo4.bin
DEPENDPATH += .
INCLUDEPATH += . ../../../src/ ../../demo2/
DESTDIR = ../

MOC_DIR =  ../../../tmp/
RCC_DIR =  ../../../tmp/
UI_DIR =  ../../../tmp/
OBJECTS_DIR = ../../../tmp/


# Input

unix:LIBS += -L../ -lqmdi  -lqexedit
win32:LIBS += ../qmdilib/qmdi.lib ../qexedit/qexedit.lib

RESOURCES += demo4.qrc
HEADERS += mainwindow4.h 
SOURCES += mainwindow4.cpp  main4.cpp

