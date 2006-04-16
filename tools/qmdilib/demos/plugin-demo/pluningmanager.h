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
#include "configdialog.h"
#include "ui_plugin_list.h"

class qmdiHost;
class qmdiServer;

class IPlugin;
class PluginModel;
class QStandardItemModel;

class PluginManager: public QObject
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
	
private:
	QList<IPlugin*>		plugins;
	QStandardItemModel	*pluginModel;
	qmdiHost		*mdiHost;
	qmdiServer		*mdiServer;
	ConfigDialog		dlg;
	Ui::plugin_list ui;
};

#endif
