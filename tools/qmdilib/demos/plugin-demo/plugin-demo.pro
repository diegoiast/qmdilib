include ( ../common/common.pri ) 
include ( ../common/demos.pri ) 
DESTDIR = ./

TEMPLATE = app
TARGET = plugin-demo
FORMS = plugin_list.ui plugins/editor/editor_cfg.ui plugins/help/help_cfg.ui
HEADERS += pluginmanager.h \
 pluginmodel.h \
 iplugin.h \
 configdialog.h \
 plugins/richtext/richtextwidget.h \
 plugins/richtext/richtextclient.h \
 plugins/richtext/richtext_plg.h \
 plugins/editor/qexeditor2.h \
 plugins/editor/editor_plg.h \
 plugins/help/help_plg.h 
 
SOURCES += pluginmanager.cpp \
 main6.cpp \
 iplugin.cpp \
 pluginmodel.cpp \
 configdialog.cpp \
 plugins/richtext/richtextwidget.cpp \
 plugins/richtext/richtextclient.cpp \
 plugins/richtext/richtext_plg.cpp \
 plugins/editor/qexeditor2.cpp \
 plugins/editor/editor_plg.cpp \
 plugins/help/help_plg.cpp

RESOURCES += richtext.qrc

CONFIG += debug_and_release
