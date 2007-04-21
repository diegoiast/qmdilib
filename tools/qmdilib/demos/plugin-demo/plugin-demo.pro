# $Id$

include ( ../common/common.pri )
include ( ../common/demos.pri )

CONFIG += debug
DESTDIR  = ./
TEMPLATE = app
TARGET   = plugin-demo
FORMS    = plugin_list.ui
HEADERS += pluginmanager.h	pluginmodel.h	iplugin.h	configdialog.h		plugins/richtext/richtext.h	plugins/richtext/richtext_browser.h
SOURCES += pluginmanager.cpp	pluginmodel.cpp	iplugin.cpp	configdialog.cpp	plugins/richtext/richtext.cpp	plugins/richtext/richtext_browser.cpp main6.cpp
RESOURCES += richtext.qrc

# plugins
HEADERS += plugins/editor/editor_plg.h		plugins/help/help_plg.h		plugins/richtext/richtext_plg.h
SOURCES += plugins/editor/editor_plg.cpp	plugins/help/help_plg.cpp	plugins/richtext/richtext_plg.cpp
FORMS   += plugins/editor/editor_cfg.ui		plugins/help/help_cfg.ui
