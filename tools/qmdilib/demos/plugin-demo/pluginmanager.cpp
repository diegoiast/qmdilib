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
#include "configdialog.h"

/**
 * \brief A class which manages a list of plugins and merges their menus and toolbars to the main application
 * \author Diego Iastrubni (elcuco@kde.org)
 *
 */


PluginManager::PluginManager()
{
	newFilePopup	= new QMenu  ( tr("New..."), NULL );
	actionOpen	= new QAction( tr("Open..."), NULL  );
	actionClose	= new QAction( tr("Close"), NULL  );
	actionQuit	= new QAction( tr("Ex&it"), this );
	actionConfig	= new QAction( tr("&Config"), this );
	
	actionOpen->setIcon( QIcon(":/images/open.png") );
	actionOpen->setShortcut( tr("Ctrl+O") );
	actionClose->setShortcut( tr("Ctrl+w") );

	connect( actionConfig, SIGNAL(triggered()), this, SLOT(on_actionConfigure_triggered()));
	connect( actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));
	connect( actionQuit, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
	initGUI();
	
	configDialog = NULL;
}

PluginManager::~PluginManager()
{
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
	menus[tr("&File")]->addAction(actionClose);
	menus[tr("&File")]->addSeparator();
	menus[tr("&File")]->addAction(actionQuit);
	menus[tr("&Edit")];
	menus[tr("&Search")];
	menus[tr("&Navigation")];
	menus[tr("&Settings")]->addAction( actionConfig );
	menus[tr("&Help")];
	
	toolbars[tr("main")]->addAction(actionOpen);
	toolbars[tr("main")]->addAction(actionConfig);

	tabWidget = new qmdiTabWidget(this);
	updateGUI();

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
#if 0
	// for each selected file, try to open it
	foreach( e, s )
	{
		IPlugin *bestPlugin = NULL;

		// who can open this file...?
		int j = -1;
		int k = -1;
		foreach( p, plugins )
		{
			if (!p->enabled)
				continue;

			// is this plugin better then the selected?
			j = p->canOpenFile(e);

			if (j > k)
			{
				bestPlugin = p;
				k = bestPlugin->canOpenFile(e);
			}
		}

		// if found, ask it to open the file
		if (bestPlugin)
			bestPlugin->openFile( e );
		/*
		else
			no plugin can handle this file,
			this should not happen, and usually means a bug
		*/
	} /* foreach( e, s ) */
#else
	openFiles(s);
#endif
}

void PluginManager::on_actionQuit_triggered()
{
	this->close();
}

void PluginManager::on_actionConfigure_triggered()
{
	if (!configDialog)
	{
		configDialog = new ConfigDialog( this );
		configDialog->setManager( this );
// 		configDialog->adjustSize();
	}

	configDialog->show();
	configDialog->setFocus();
}

bool PluginManager::openFile( QString fileName )
{
	// who can open this file...?
	IPlugin *bestPlugin = NULL;
	IPlugin *p;
	int j = -1;
	int k = -1;
	
	foreach( p, plugins )
	{
		if (!p->enabled)
			continue;

			// is this plugin better then the selected?
		j = p->canOpenFile(fileName);

		if (j > k)
		{
			bestPlugin = p;
			k = j; //bestPlugin->canOpenFile(fileName);
		}
	}

	// if found, ask it to open the file
	if (bestPlugin)
		return bestPlugin->openFile( fileName );
	else
	/*
		no plugin can handle this file,
		this should not happen, and usually means a bug
	*/
		return false;
}

bool PluginManager::openFiles(QStringList fileNames )
{
	QString s;
	bool b = true;
	foreach( s, fileNames )
	{
		b = b && openFile( s );
	}
	
	return b;
}
