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
 plugins/richtext/richtext.h \
 plugins/richtext/qexrichtextbrowser.h \
 plugins/editor/qexeditor2.h \
 plugins/editor/editor_plg.h \
 plugins/help/help_plg.h \
 plugins/richtext/richtext_plg.h
SOURCES += pluginmanager.cpp \
 pluginmodel.cpp \
 iplugin.cpp \
 configdialog.cpp \
 plugins/richtext/richtext.cpp \
 plugins/richtext/qexrichtextbrowser.cpp \
 plugins/editor/qexeditor2.cpp \
 main6.cpp \
 plugins/editor/editor_plg.cpp \
 plugins/help/help_plg.cpp \
 plugins/richtext/richtext_plg.cpp
RESOURCES += richtext.qrc
CONFIG += debug_and_release
