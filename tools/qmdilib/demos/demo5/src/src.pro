TEMPLATE = app
TARGET = demo5
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

RESOURCES += demo5.qrc
HEADERS += mainwindow5.h 
SOURCES += mainwindow5.cpp  main5.cpp

