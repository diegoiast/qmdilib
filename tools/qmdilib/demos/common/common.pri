INCLUDEPATH	+= ../../src/ ../common/ .
MOC_DIR		+= ../../tmp/
RCC_DIR		+= ../../tmp/
UI_DIR		+= ../../tmp/
OBJECTS_DIR	+= ../../../tmp/

# Input
RESOURCES += ../common/common.qrc

# qmdilib
HEADERS += ../../src/actiongroup.h \
           ../../src/actiongrouplist.h \
           ../../src/qmdiclient.h \
           ../../src/qmdihost.h \
           ../../src/qmdiserver.h \
           ../../src/qmditabwidget.h

SOURCES += ../../src/actiongroup.cpp \
           ../../src/actiongrouplist.cpp \
           ../../src/qmdiclient.cpp \
           ../../src/qmdihost.cpp \
           ../../src/qmdiserver.cpp \
           ../../src/qmditabwidget.cpp
