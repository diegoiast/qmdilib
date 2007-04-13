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
#include <QTabWidget>
#include <QToolButton>
#include <QStatusBar>

#include "qmdihost.h"
#include "qmdiserver.h"
#include "qmditabwidget.h"
#include "iplugin.h"
#include "pluginmanager.h"
#include "pluginmodel.h"

/**
 * \brief A class which manages a list of plugins and merges their menus and toolbars to the main application
 * \author Diego Iastrubni (elcuco@kde.org)
 * 
 */


PluginManager::PluginManager()
{
	newFilePopup	= new QMenu  ( tr("New..."), NULL );
	actionOpen	= new QAction( tr("Open..."), NULL  );
	actionQuit	= new QAction( tr("Ex&it"), this );
	actionConfig	= new QAction( tr("&Config"), this );
	
// 	connect( actionConfig, SIGNAL(triggered()), pluginManager, SLOT(configurePlugins()));
	connect( actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));
	connect( actionQuit, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
	initGUI();
}

PluginManager::~PluginManager()
{
}

void PluginManager::updateGUI2()
{
	updateGUI( this );
}

void PluginManager::addPlugin( IPlugin *newplugin )
{
	plugins << newplugin;
	
	if (!newplugin)
		return;
	
	newplugin->mdiServer = tabWidget;
	if (newplugin->alwaysEnabled)
		newplugin->autoEnabled = true;
	
	if (newplugin->autoEnabled)
	{
		newplugin->enabled = true;
		mergeClient( newplugin );
	}
	
	// lets see how much "new" actions we have
	IPlugin *p;
	QAction *a;
	QActionGroup *ag;
	
	foreach( p, plugins )
	{
		if (!p->enabled)
			continue;
		
		ag =  p->newFileActions();
		if (!ag)
			continue;
			
		foreach( a, ag->actions() )
		{
			newFilePopup->addAction( a );
		}
	}
	
}

void PluginManager::removePlugin( IPlugin *oldplugin )
{
}

void PluginManager::initGUI()
{
	menus[tr("&File")]->addMenu( newFilePopup );
	menus[tr("&File")]->addAction( actionOpen );
	menus[tr("&File")]->addSeparator();
	menus[tr("&File")]->setMergePoint();
	menus[tr("&File")]->addSeparator();
	menus[tr("&File")]->addAction(actionQuit);
	menus[tr("&Edit")];
	menus[tr("&Search")];
	menus[tr("&Navigation")];
	menus[tr("Se&ttings")]->addAction( actionConfig );
	menus[tr("&Help")];

	toolbars[tr("main")]->addAction(actionOpen);
	toolbars[tr("main")]->addAction(actionConfig);
	
	tabWidget = new qmdiTabWidget(this);
	updateGUI2();
	
	QToolButton *tabCloseBtn = new QToolButton(tabWidget);
	tabCloseBtn->setAutoRaise( true );
	connect( tabCloseBtn, SIGNAL(clicked()), this, SLOT(closeClient()));
	tabCloseBtn->setIcon(QIcon(":images/closetab.png"));
	tabWidget->setCornerWidget( tabCloseBtn, Qt::TopRightCorner  );
	setCentralWidget( tabWidget );	
	statusBar()->showMessage("Welcome - feel free to configure the GUI to your needs",5000);
}

void PluginManager::closeClient()
{
	qmdiClient *client = dynamic_cast<qmdiClient*>( tabWidget->currentWidget() );
	if (client == NULL)
		tabWidget->currentWidget()->deleteLater();
	else
		client->closeClient();
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
				j = p->canOpenFile(e);
				
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
	} /* foreach( e, s ) */
}

void PluginManager::on_actionQuit_triggered()
{
	this->close();
}
