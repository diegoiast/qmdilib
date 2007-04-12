/**
 * \file pluginmanager.cpp
 * \brief Implementation of the PluginManager class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL
 * \see PluginManager
 */

// $Id$

#include <QStandardItemModel>
#include <QMainWindow>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QActionGroup>

#include "qmdihost.h"
#include "qmdiserver.h"
#include "iplugin.h"
#include "pluginmanager.h"
#include "pluginmodel.h"

/**
 * \brief A class which manages a list of plugins and merges their menus and toolbars to the main application
 * \author Diego Iastrubni (elcuco@kde.org)
 * 
 */


PluginManager::PluginManager( qmdiHost *host, qmdiServer *server )
{
	mdiServer = server;
	server->mdiHost = host;
	
	newFilePopup	= new QMenu( tr("New..."), NULL );
	actionOpen	= new QAction( tr("Open... (plugin)"), NULL  );
	
	connect( actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));
// 	newFilePopup->addAction( actionNew );
	
	menus["&File"]->addMenu( newFilePopup );
// 	menus["&File"]->addAction( actionNew );
	menus["&File"]->addAction( actionOpen );
	if (server->mdiHost)
		server->mdiHost->mergeClient( this );
}

PluginManager::~PluginManager()
{
}

void PluginManager::addPlugin( IPlugin *newplugin )
{
	plugins << newplugin;
	
	if (!newplugin)
		return;

	newplugin->mdiServer = mdiServer;
	if (newplugin->alwaysEnabled)
		newplugin->autoEnabled = true;
	
	if (newplugin->autoEnabled)
	{
		newplugin->enabled = true;
		mdiServer->mdiHost->mergeClient( newplugin );
	}
	
	// lets see how much "new" actions we have
	IPlugin *p;
	QAction *a;
// 	QActionGroup *ag = new QActionGroup
	
	foreach( p, plugins )
	{
		if (!p->enabled)
			continue;
		QActionGroup *ag =  p->newFileActions();
		if (ag)
			foreach( a, ag->actions() )
			{
				newFilePopup->addAction( a );
			}
	}
	
}

void PluginManager::removePlugin( IPlugin *oldplugin )
{
}

void PluginManager::on_actionOpen_triggered()
{
	static QString workingDir = "";
	QString extens, e;
	QStringList extensAvailable;
	IPlugin *p;
	
	// get list of available extensions to open 
	// from each plugin
	foreach( p, plugins )
	{
		if (!p->enabled)
			continue;
		extensAvailable << p->myExtensions();
	}
	
	int j = extensAvailable.size();
	for (int i = 0; i < j; ++i)
	{
		extens += extensAvailable.at(i);
		if (i<j-1)
			extens += ";;";
	}


	QStringList s = QFileDialog::getOpenFileNames(
		NULL,
		tr("Choose a file"),
		workingDir,
		extens
	);

	if (s.isEmpty())
		return;

	// for each selected file, try to open it
	foreach( e, s )
	{
		IPlugin *bestPlugin = NULL;
		
		// who can open this file...?
		int j = 0;
		int k = 0;
		foreach( p, plugins )
		{
			if (!p->enabled)
				continue;
			
			if (!bestPlugin)
				bestPlugin = p;
			else
			{
				// is this plugin better then the selected?
				int j = p->canOpenFile(e);
				
				if (j > k)
				{
					bestPlugin = p;
					k = bestPlugin->canOpenFile(e);
				}
			}
		}

		// if found, ask it to open the file
		if (bestPlugin)
			bestPlugin->openFile( e );
	}
}
