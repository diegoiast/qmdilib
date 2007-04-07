# $Id$

include ( ../common/common.pri )
include ( ../common/demos.pri )

DESTDIR  = ./
TEMPLATE = app
TARGET   = plugin-demo
HEADERS += pluginmanager.h iplugin.h mainwindow6.h 
SOURCES += pluginmanager.cpp iplugin.cpp  mainwindow6.cpp  main6.cpp 

# plugins
HEADERS += plugins/editor/editor.h	plugins/help/help.h
SOURCES += plugins/editor/editor.cpp	plugins/help/help.cpp
FORMS   += plugins/editor/editor_cfg.ui	plugins/help/help_cfg.ui