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
 * \author Diego Iastrubni <elcuco@kde.org>
 * 
 */
PluginManager::PluginManager( qmdiHost *host, qmdiServer *server )
{
	mdiHost = host;
	mdiServer = server;

	QWidget *w = new QWidget;
	ui.setupUi( w );
	w->setObjectName("config");
	
	pluginModel = new PluginModel( &plugins );
	ui.listWidget->setModel( pluginModel );
	connect( ui.cbEnabled, SIGNAL(toggled(bool)), this, SLOT(on_cbEnabled_togglged(bool)));
	connect( ui.btnAbout, SIGNAL(clicked()), this, SLOT(on_aboutPlugin_clicked()));
	
	connect( ui.listWidget, SIGNAL(activated(QModelIndex)), this, SLOT(on_listWidget_activated(QModelIndex)));
	connect( ui.listWidget, SIGNAL(entered(QModelIndex)), this, SLOT(on_listWidget_activated(QModelIndex)));
	connect( ui.listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(on_listWidget_activated(QModelIndex)));

	dlg.addPage( w, QIcon("images/config.png") );
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
}

// called when the user seleccts a new plugin
void PluginManager::on_listWidget_activated(const QModelIndex & index )
{
	IPlugin *plugin = plugins[index.row()];

	if (!plugin)
		return;

	ui.pluginName->setText( plugin->getName() );
	ui.pluginName->setCursorPosition( 0 );
	
	ui.authorName->setText( plugin->getAuthor() );
	ui.authorName->setCursorPosition( 0 );
	
	ui.versionString->setText( plugin->getsVersion() );
	ui.versionString->setCursorPosition( 0 );

	ui.cbEnabled->setEnabled( !plugin->alwaysEnabled );
	ui.cbEnabled->setChecked( plugin->isEnabled() );
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
		// TODO add also the configuration dialog for this plugin
		mdiHost->mergeClient( plugin );
	}
	else
	{
		// TODO remove also the configuration dialog for this plugin
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

	dlg.raise();
	dlg.setFocus();
}
