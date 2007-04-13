# $Id$

include ( ../common/common.pri )
include ( ../common/demos.pri )

DESTDIR  = ./
TEMPLATE = app
TARGET   = plugin-demo
HEADERS += 		pluginmanager.h		iplugin.h	plugins/richtext/richtext.h	plugins/richtext/richtext_browser.h
SOURCES += main6.cpp	pluginmanager.cpp	iplugin.cpp	plugins/richtext/richtext.cpp	plugins/richtext/richtext_browser.cpp
RESOURCES += richtext.qrc

# plugins
HEADERS += plugins/editor/editor_plg.h		plugins/help/help_plg.h		plugins/richtext/richtext_plg.h
SOURCES += plugins/editor/editor_plg.cpp	plugins/help/help_plg.cpp	plugins/richtext/richtext_plg.cpp
FORMS   += plugins/editor/editor_cfg.ui		plugins/help/help_cfg.ui
