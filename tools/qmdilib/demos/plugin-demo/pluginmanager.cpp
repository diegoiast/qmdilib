/**
 * \file pluginmanager.cpp
 * \brief Implementation of the PluginManager class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
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
#include <QApplication>
#include <QSettings>

#include "qmdihost.h"
#include "qmdiserver.h"
#include "qmditabwidget.h"
#include "qmdiworkspace.h"
#include "iplugin.h"
#include "pluginmanager.h"
#include "configdialog.h"

/**
 * \brief A class which manages a list of plugins and merges their menus and toolbars to the main application
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *
 * The plugin managed is a mdi host which can load menus and toolbars from 
 * a the selected mdi client mdi server, and also maintain another set of mdi
 * clients, which have no real GUI, but their menus and toolbars are merged to the main 
 * host.
 * 
 * This can be used to enable or disable functionality of the application on run 
 * time - by enabling or disabling IPlugin objetcs.
 * 
 * Each plugin has defines a set 
 */


PluginManager::PluginManager()
{
	configDialog = NULL;

	newFilePopup	= new QMenu( tr("New..."), this );
	actionOpen	= new QAction( tr("Open..."), this );
	actionClose	= new QAction( tr("Close"), this );
	actionQuit	= new QAction( tr("Ex&it"), this );
	actionConfig	= new QAction( tr("&Config"), this );
	actionNextTab	= new QAction( tr("&Next tab"), this );
	actionPrevTab	= new QAction( tr("&Previous tab"), this );

	actionNextTab->setEnabled( false );
	actionPrevTab->setEnabled( false );
	actionClose->setEnabled( false );
	
	newFilePopup->setObjectName("PluginManager::newFilePopup");
	actionOpen->setObjectName("PluginManager::actionOpen");
	actionClose->setObjectName("PluginManager::actionClose");
	actionQuit->setObjectName("PluginManager::actionQuit");
	actionConfig->setObjectName("PluginManager::actionConfig");
	actionNextTab->setObjectName("PluginManager::actionNextTab");
	actionPrevTab->setObjectName("PluginManager::actionPrevTab");
	
	actionOpen->setIcon( QIcon(":/trolltech/styles/commonstyle/images/diropen-32.png") );
	actionOpen->setShortcut( QKeySequence("Ctrl+O") );
	actionClose->setIcon( QIcon(":/trolltech/styles/commonstyle/images/standardbutton-cancel-32.png") );
	actionClose->setShortcut( QKeySequence("Ctrl+w") );
	
	actionNextTab->setShortcut( QKeySequence("Alt+Right") );
	actionPrevTab->setShortcut( QKeySequence("Alt+Left") );

	connect( actionConfig, SIGNAL(triggered()), this, SLOT(on_actionConfigure_triggered()));
	connect( actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));
	connect( actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_triggered()));
	connect( actionNextTab, SIGNAL(triggered()), this, SLOT(on_actionNext_triggered()));
	connect( actionPrevTab, SIGNAL(triggered()), this, SLOT(on_actionPrev_triggered()));
	initGUI();
}

PluginManager::~PluginManager()
{
	if (settingsManager)
	{
		saveSettings();
		delete settingsManager;
	}
}

int PluginManager::tabForFileName( QString fileName )
{
	if (fileName.isEmpty())
		return -1;
		
	for( int i=0; i<tabWidget->count(); i++ )
	{
		qmdiClient *c = dynamic_cast< qmdiClient *> (tabWidget->widget(i));
		if (!c)
			continue;
			
		if (c->mdiClientFileName() == fileName)
			return i;
	}

	return -1;
}

void	PluginManager::setNativeSettingsManager( const QString &organization, const QString &application )
{
	settingsManager = new QSettings( organization, application );
}

void	PluginManager::setFileSettingsManager( const QString &fileName )
{
	settingsManager = new QSettings( fileName, QSettings::IniFormat );
}

void PluginManager::restoreSettings()
{
	if (!settingsManager)
		return;
	
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QApplication::processEvents();
	
	// restore window location
	settingsManager->beginGroup("mainwindow");
	if (settingsManager->contains("location"))
		move( settingsManager->value("location").toPoint() );
	if (settingsManager->contains("size"))
		resize( settingsManager->value("size").toSize() );
	settingsManager->endGroup();
	
	show();
	statusBar()->showMessage( tr("Loading files...") ,5000);
	QApplication::restoreOverrideCursor();
	QApplication::processEvents();

	// restore opened files
	settingsManager->beginGroup("files");
	foreach( QString s, settingsManager->childKeys() )
	{
		if (!s.startsWith("file"))
			continue;
		QString fileName = settingsManager->value(s).toString();
		statusBar()->showMessage( tr("Loading file %1").arg(fileName) ,5000);
		QApplication::processEvents();
		openFile(fileName); 
	}
	
	// re-select the current tab
	QString s = settingsManager->value( "current").toString();
	if (!s.isEmpty())
		openFile( s );
	statusBar()->clearMessage();
	settingsManager->endGroup();
}

void PluginManager::saveSettings()
{
	if (!settingsManager)
		return;
	
	// main window state
	settingsManager->beginGroup("mainwindow");
	settingsManager->setValue( "size", size() );
	settingsManager->setValue( "location", pos() );
	settingsManager->setValue( "maximized", isMaximized() );
	settingsManager->setValue( "state", saveGeometry() );
	settingsManager->endGroup();
	
	// store saved files
	settingsManager->remove("files");	// remove all old loaded files	
	if (tabWidget->count()!=0)
	{
		qmdiClient *c = NULL;
		settingsManager->beginGroup("files");
		for ( int i=0; i<tabWidget->count(); i++ )
		{
			c = dynamic_cast< qmdiClient *> (tabWidget->widget(i));
			if (!c)
				continue;
			settingsManager->setValue( QString("file%1").arg(i), c->mdiClientFileName() );
		}
		
		c = dynamic_cast< qmdiClient *> (tabWidget->currentWidget());
		if (c)
			settingsManager->setValue( "current", c->mdiClientFileName() );
	}
	settingsManager->endGroup();
	
	// let each ones of the plugins save it's state
	foreach( IPlugin *p, plugins )
	{
		p->saveConfig( *settingsManager );
	}
	
	settingsManager->sync();
}

void PluginManager::addPlugin( IPlugin *newplugin )
{
	plugins << newplugin;
	if (settingsManager)
		newplugin->loadConfig( *settingsManager );
	
	if (!newplugin)
		return;
	
	newplugin->mdiServer = dynamic_cast<qmdiServer*>(tabWidget);
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
	Q_UNUSED( oldplugin );
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
	menus[tr("Se&ttings")]->addAction( actionConfig );
	menus[tr("Se&ttings")]->addSeparator();
	menus[tr("Se&ttings")]->addAction( actionNextTab );
	menus[tr("Se&ttings")]->addAction( actionPrevTab );
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

	// get list of available extensions to open from each plugin
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
	
	openFiles(s);
}

void PluginManager::on_actionClose_triggered()
{
	tabWidget->tryCloseClient( tabWidget->currentIndex() );
	
	// TODO fix this to be calculated when tabs are open
	//      or closed, do this via a signal from QTabWidget (qmdiServer?)
	int widgetsCount = tabWidget->count();
	actionClose->setEnabled( widgetsCount != 0 );
	actionNextTab->setEnabled( widgetsCount > 1 );
	actionPrevTab->setEnabled( widgetsCount > 1 );
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
	}
	
	configDialog->show();
	configDialog->setFocus();
}

void PluginManager::on_actionPrev_triggered()
{
	int  i = tabWidget->currentIndex();
	if (i==0)
		return;
		
	i--;
	tabWidget->setCurrentIndex( i );
}

void PluginManager::on_actionNext_triggered()
{
	int  i = tabWidget->currentIndex();
	if (i==tabWidget->count())
		return;
		
	i++;
	tabWidget->setCurrentIndex( i );
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
	
	actionClose->setEnabled( true );
	k = tabForFileName( fileName );
	if (k != -1)
	{	// see if it's already open
		tabWidget->setCurrentIndex( k );
		return true;
	}
	else if (bestPlugin)
		// if not open, ask best plugin to open the file
		return bestPlugin->openFile( fileName );
	else
		// no plugin can handle this file,
		// this should not happen, and usually means a bug
		return false;
}

bool PluginManager::openFiles( QStringList fileNames )
{
	QString s;
	bool b = true;
	foreach( s, fileNames )
	{
		b = b && openFile( s );
		QApplication::processEvents();
	}

	int widgetsCount = tabWidget->count();
	actionClose->setEnabled( widgetsCount != 0 );
	actionNextTab->setEnabled( widgetsCount > 1 );
	actionPrevTab->setEnabled( widgetsCount > 1 );
	
	return b;
}
