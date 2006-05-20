//
// C++ Interface: pluningmanager
//
// Description:
//
//
// Author: Diego Iastrubni <elcuco@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLUNINGMANAGER_H
#define PLUNINGMANAGER_H

#include <QObject>
#include "qmdiclient.h"
#include "configdialog.h"
#include "ui_plugin_list.h"

class qmdiHost;
class qmdiServer;

class IPlugin;
class PluginModel;
class QStandardItemModel;

class PluginManager: public QObject, public qmdiClient
{
	Q_OBJECT
	
public:
	PluginManager( qmdiHost *host, qmdiServer *server );
	virtual ~PluginManager();

public slots:
	void configurePlugins();
	void addPlugin( IPlugin *newplugin );
	void on_listWidget_activated(const QModelIndex & index );
	void on_cbEnabled_togglged( bool checked );
	void on_aboutPlugin_clicked();
	void on_mainOpen_clicked();

	void on_apply_dialog();
	void on_close_dialog();

private:
	QList<IPlugin*>		plugins;
	QStandardItemModel	*pluginModel;
	qmdiHost		*mdiHost;
	qmdiServer		*mdiServer;
	ConfigDialog		dlg;
	Ui::plugin_list		ui;

	QAction			*mainOpen;
// 	QStringList		extensAvailable;
};

#endif
