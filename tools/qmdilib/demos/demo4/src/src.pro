TEMPLATE = app
unix:TARGET = demo4.bin
win32:TARGET = demo4
DEPENDPATH += .
INCLUDEPATH += . ../../../src/ ../../common/
DESTDIR = ../

MOC_DIR =  ../../../tmp/
RCC_DIR =  ../../../tmp/
UI_DIR =  ../../../tmp/
OBJECTS_DIR = ../../../tmp/


# Input

unix:LIBS += -L../ -lqmdi  -lqexedit
win32:LIBS += -L../ -lqmdi0 -lqexedit0

RESOURCES += demo4.qrc
HEADERS += mainwindow4.h 
SOURCES += mainwindow4.cpp  main4.cpp

