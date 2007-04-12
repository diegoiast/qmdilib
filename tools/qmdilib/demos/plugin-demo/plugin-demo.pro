# $Id$

include ( ../common/common.pri )
include ( ../common/demos.pri )

DESTDIR  = ./
TEMPLATE = app
TARGET   = plugin-demo
HEADERS += pluginmanager.h iplugin.h mainwindow6.h plugins/richtext/richtext.h plugins/richtext/richtext_browser.h
SOURCES += pluginmanager.cpp iplugin.cpp  mainwindow6.cpp  main6.cpp plugins/richtext/richtext.cpp plugins/richtext/richtext_browser.cpp
RESOURCES += richtext.qrc

# plugins
HEADERS += plugins/editor/editor.h	plugins/help/help.h		plugins/richtext/richtext_plg.h
SOURCES += plugins/editor/editor.cpp	plugins/help/help.cpp		plugins/richtext/richtext_plg.cpp
FORMS   += plugins/editor/editor_cfg.ui	plugins/help/help_cfg.ui