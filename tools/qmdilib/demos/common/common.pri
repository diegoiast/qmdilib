# $Id$

INCLUDEPATH	+= ../../src/ ../common/ .
MOC_DIR		+= ../../tmp/
RCC_DIR		+= ../../tmp/
UI_DIR		+= ../../tmp/
OBJECTS_DIR	+= ../../tmp/

# Input
RESOURCES += ../common/common.qrc

# qmdilib
HEADERS += ../../src/qmdiactiongroup.h \
           ../../src/qmdiactiongrouplist.h \
           ../../src/qmdiclient.h \
           ../../src/qmdihost.h \
           ../../src/qmdiserver.h \
           ../../src/qmditabwidget.h \
           ../../src/qmdiworkspace.h \
           ../../src/qmdimainwindow.h

SOURCES += ../../src/qmdiactiongroup.cpp \
           ../../src/qmdiactiongrouplist.cpp \
           ../../src/qmdiclient.cpp \
           ../../src/qmdihost.cpp \
           ../../src/qmdiserver.cpp \
           ../../src/qmdiworkspace.cpp \
           ../../src/qmditabwidget.cpp
