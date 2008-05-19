#ifndef PLUNINGMANAGER_H
#define PLUNINGMANAGER_H

/**
 * \file pluginmanager.h
 * \brief Definition of the PluginManager class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL 2 or 3
 * \see PluginManager
 */

// $Id$

#include <QMainWindow>
#include "qmdihost.h"

class QPopupMenu;
class QSettings;

class qmdiHost;
class qmdiTabWidget;
class IPlugin;
class ConfigDialog;

class PluginManager: public QMainWindow, public qmdiHost
{
	Q_OBJECT
	
public:
	PluginManager();
	virtual ~PluginManager();
	int tabForFileName( QString fileName );
	void setNativeSettingsManager( const QString &organization=QString(), const QString &application=QString() );
	void setFileSettingsManager( const QString &fileName=QString() );
	void restoreSettings();
	void saveSettings();
	void updateActionsStatus();
	bool openFile(QString fileName, int x=-1, int y=-1, int z=-1 );
	bool openFiles(QStringList fileNames );

public slots:
	void addPlugin( IPlugin *newplugin );
	void removePlugin( IPlugin *oldplugin );
	void enablePlugin( IPlugin *plugin );
	void disablePlugin( IPlugin *plugin );
	
	void closeClient();
	
	void on_actionOpen_triggered();
	void on_actionClose_triggered();
	void on_actionConfigure_triggered();
	void on_actionQuit_triggered();
	void on_actionPrev_triggered();
	void on_actionNext_triggered();
	
private:
	void initGUI();
	
public:	
	QList<IPlugin*>	plugins;
	
	qmdiTabWidget	*tabWidget;
	QAction		*actionOpen;
	QAction		*actionConfig;
	QAction		*actionClose;
	QAction		*actionQuit;
	QAction		*actionNextTab;
	QAction		*actionPrevTab;
	QMenu		*newFilePopup;
	
	ConfigDialog	*configDialog;
	QSettings	*settingsManager;
};

#endif
