TEMPLATE = app
TARGET += 
DEPENDPATH += .

MOC_DIR =  ../../tmp/
RCC_DIR =  ../../tmp/
UI_DIR =  ../../tmp/
OBJECTS_DIR = ../../tmp/

INCLUDEPATH += . ../../src/
DEPENDPATH = . 
INCLUDEPATH += ../mdi-tab \
               plugins/editor \
               plugins/help \
               ../../src \
               . 

RESOURCES += ../mdi-tab/mdi-tab.qrc


FORMS += plugins/editor/editor_cfg.ui \
         plugins/help/help_cfg.ui \
         plugin_list.ui 

HEADERS += configdialog.h \
           plugins/help/help.h \
           plugins/editor/editor.h \
           iplugin.h \
           mainwindow3.h \
           pluningmanager.h \
           pluginmodel.h \
           ../../src/actiongroup.h \
           ../../src/actiongrouplist.h \
           ../../src/qmdiclient.h \
           ../../src/qmdihost.h \
           ../../src/qmdiserver.h \
           ../../src/qmditabwidget.h \
           ../mdi-tab/helpbrowse.h \
           ../mdi-tab/qexeditor.h

SOURCES += configdialog.cpp \
           plugins/help/help.cpp \
           plugins/editor/editor.cpp \
           iplugin.cpp \
           main3.cpp \
           mainwindow3.cpp \
           pluningmanager.cpp \
           pluginmodel.cpp \
           ../../src/actiongroup.cpp \
           ../../src/actiongrouplist.cpp \
           ../../src/qmdiclient.cpp \
           ../../src/qmdihost.cpp \
           ../../src/qmdiserver.cpp \
           ../../src/qmditabwidget.cpp \
           ../mdi-tab/helpbrowse.cpp \
           ../mdi-tab/qexeditor.cpp 
