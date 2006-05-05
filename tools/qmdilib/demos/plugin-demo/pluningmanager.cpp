//
// C++ Implementation: pluningmanager
//
// Description: 
//
//
// Author: Diego Iastrubni <elcuco@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QStandardItemModel>
#include <QMainWindow>

#include "qmdihost.h"
#include "qmdiserver.h"
#include "iplugin.h"
#include "pluningmanager.h"
#include "pluginmodel.h"

/**
 * \brief A class which manages a list of plugins and merges their menus and toolbars to the main application
 * \author Diego Iastrubni (elcuco@kde.org)
 * 
 */
PluginManager::PluginManager( qmdiHost *host, qmdiServer *server )
{
	mdiHost = host;
	mdiServer = server;

	QWidget *w = new QWidget;
	ui.setupUi( w );
	w->setObjectName("config");

	connect( dlg.applyButton, SIGNAL(clicked()), this, SLOT(on_apply_dialog()));
	connect( dlg.closeButton, SIGNAL(clicked()), this, SLOT(on_close_dialog()));

	// define the main configuration panel
	pluginModel = new PluginModel( &plugins );
	ui.listWidget->setModel( pluginModel );
	connect( ui.cbEnabled, SIGNAL(toggled(bool)), this, SLOT(on_cbEnabled_togglged(bool)));
	connect( ui.btnAbout, SIGNAL(clicked()), this, SLOT(on_aboutPlugin_clicked()));
	connect( ui.listWidget, SIGNAL(activated(QModelIndex)), this, SLOT(on_listWidget_activated(QModelIndex)));
	connect( ui.listWidget, SIGNAL(entered(QModelIndex)), this, SLOT(on_listWidget_activated(QModelIndex)));
	connect( ui.listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(on_listWidget_activated(QModelIndex)));

	dlg.addPage( w, QIcon("images/config.png") );

	/*
	mainOpen = new QAction( "New Multi Open", this );
	menus["&File"]->addAction( mainOpen );
	connect( mainOpen, SIGNAL(triggered()), this, SLOT(on_mainOpen_clicked()));
	if (mdiHost)
		mdiHost->mergeClient( this );
	*/
}

PluginManager::~PluginManager()
{
}

void PluginManager::configurePlugins()
{
	if (dlg.isVisible())
		dlg.hide();
	else
		dlg.show();
}

void PluginManager::addPlugin( IPlugin *newplugin )
{
	plugins << newplugin;
	
	if (!newplugin)
		return;

	// when the checkboxes state is modified, a signal is been emited
	// we need to ignore this for this function. re-connected at the end of this
	// function
	disconnect( ui.cbEnabled, SIGNAL(toggled(bool)), this, SLOT(on_cbEnabled_togglged(bool)));
	
	QModelIndex parent = pluginModel->index(0, 0, parent);
	int count = pluginModel->columnCount();
	
	pluginModel->insertRows(count, 1, parent);
	pluginModel->insertColumns(count, 1, parent);
	QModelIndex index = pluginModel->index(count, 0, parent);
	pluginModel->setData(index, newplugin->getName() );

	newplugin->mdiServer = mdiServer;
	if (newplugin->alwaysEnabled)
		newplugin->autoEnabled = true;
	
	if (newplugin->autoEnabled)
	{
		newplugin->enabled = true;
		mdiHost->mergeClient( newplugin );
	
		if (newplugin->getConfigDialog() != NULL)
			dlg.addPage( newplugin->getConfigDialog(), newplugin->getIcon() );
	}


	// ugly work arround to fix the selection of the first item
	if (plugins.count() == 1)
	{
		ui.pluginName->setText( newplugin->getName() );
		ui.pluginName->setCursorPosition( 0 );
	
		ui.authorName->setText( newplugin->getAuthor() );
		ui.authorName->setCursorPosition( 0 );
	
		ui.versionString->setText( newplugin->getsVersion() );
		ui.versionString->setCursorPosition( 0 );

		ui.cbEnabled->setEnabled( !newplugin->alwaysEnabled );
// 		TODO why does this crash?
// 		ui.cbEnabled->setChecked( newplugin->isEnabled() );
	}

	connect( ui.cbEnabled, SIGNAL(toggled(bool)), this, SLOT(on_cbEnabled_togglged(bool)));
}

// called when the user seleccts a new plugin
void PluginManager::on_listWidget_activated(const QModelIndex & index )
{
	IPlugin *plugin = plugins[index.row()];

	if (!plugin)
		return;

	// when the checkboxes state is modified, a signal is been emited
	// we need to ignore this for this function. re-connected at the end of this
	// function
	disconnect( ui.cbEnabled, SIGNAL(toggled(bool)), this, SLOT(on_cbEnabled_togglged(bool)));
	ui.pluginName->setText( plugin->getName() );
	ui.pluginName->setCursorPosition( 0 );
	
	ui.authorName->setText( plugin->getAuthor() );
	ui.authorName->setCursorPosition( 0 );
	
	ui.versionString->setText( plugin->getsVersion() );
	ui.versionString->setCursorPosition( 0 );

	ui.cbEnabled->setEnabled( !plugin->alwaysEnabled );
	ui.cbEnabled->setChecked( plugin->isEnabled() );
	connect( ui.cbEnabled, SIGNAL(toggled(bool)), this, SLOT(on_cbEnabled_togglged(bool)));
}

// called when the user checks on unchecks to enable a plugin
void PluginManager::on_cbEnabled_togglged( bool checked )
{
	QModelIndex index = ui.listWidget->currentIndex();
	IPlugin *plugin = plugins[index.row()];

	if (!plugin)
		return;

	plugin->enabled = checked;
	if (checked)
	{
		if (plugin->getConfigDialog() != NULL)
			dlg.addPage( plugin->getConfigDialog(), plugin->getIcon() );

		plugin->setData();
		mdiHost->mergeClient( plugin );
	}
	else
	{
		if (plugin->getConfigDialog() != NULL)
			dlg.removePage( plugin->getConfigDialog() );
		
		mdiHost->unmergeClient( plugin );
	}
	
	mdiHost->updateGUI( dynamic_cast<QMainWindow*>(mdiHost) );
}

void PluginManager::on_aboutPlugin_clicked()
{
	QModelIndex index = ui.listWidget->currentIndex();
	IPlugin *plugin = plugins[index.row()];

	if (!plugin)
		return;

	dlg.raise();
	dlg.setFocus();
	
	plugin->showAbout();

	// TODO still now what i want, still the dialog has no focus after the
	// about box of the plugin
	dlg.raise();
	dlg.setFocus();
}

void PluginManager::on_mainOpen_clicked()
{
	qDebug( "ping" );
}

void PluginManager::on_apply_dialog()
{
	IPlugin	*p;
	foreach( p, plugins )
	{
		if (p->enabled)
			p->getData();
	}
}

void PluginManager::on_close_dialog()
{
	IPlugin	*p;
	foreach( p, plugins )
	{
		if (p->enabled)
			p->setData();
	}
}

