#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

/**
 * \file configdialog.h
 * \brief Declaration of the configuration dialog class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL
 * \see ConfigDialog
 */

// $Id$

#include <QDialog>
#include <QIcon>
#include "ui_plugin_list.h"

class QIcon;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QListView;

class PluginManager;
class PluginModel;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog( QWidget *owner=NULL  );
	~ConfigDialog();
	void setManager( PluginManager *manager );

private slots:
	void on_aboutPlugin_clicked( bool );
	void on_configurePlugin_clicked( bool );
	void on_pluginList_activated ( const QModelIndex & index );
	void on_pluginList_clicked ( const QModelIndex & index );
	void on_pluginEnabled_toggled( bool enabled );

private:
	void updateInfo( int pluginNumber );
	bool execDialog( QWidget *w );
	
	PluginManager	*pluginManager;
	PluginModel	*pluginModel;
	
	Ui::PluginList	pluginListUi;
};

#endif
