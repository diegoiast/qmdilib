/**
 * \file main6.cpp
 * \brief Entry point of the 6st demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */

// $Id$

#include <QApplication>

#include "pluginmanager.h"
#include "plugins/editor/editor_plg.h"
#include "plugins/help/help_plg.h"
#include "plugins/richtext/richtext_plg.h"

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	PluginManager pluginManager;
	//pluginManager.initSettingsManagerNative( "Diego", "PluginManager" );
	//pluginManager.initSettingsManagerFromFile( "plugin-demo.ini" );
	//pluginManager.setNativeSettingsManager( "Diego", "PluginManager" );
	pluginManager.setFileSettingsManager( "plugin-demo.ini" );
	
	pluginManager.addPlugin( new HelpPlugin );
	pluginManager.addPlugin( new EditorPlugin );
	pluginManager.addPlugin( new RichTextPlugin );
	pluginManager.updateGUI();
	pluginManager.restoreSettings();
	
	pluginManager.show();
	return app.exec();
}

