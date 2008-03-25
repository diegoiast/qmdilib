#ifndef PLUNINGMANAGER_H
#define PLUNINGMANAGER_H

/**
 * \file pluginmanager.h
 * \brief Definition of the PluginManager class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL
 * \see PluginManager
 */

// $Id$

#include <QMainWindow>
#include "qmdihost.h"

class QPopupMenu;
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

public slots:
	void addPlugin( IPlugin *newplugin );
	void removePlugin( IPlugin *oldplugin );
	
	void closeClient();
	void on_actionOpen_triggered();
	void on_actionClose_triggered();
	void on_actionConfigure_triggered();
	void on_actionQuit_triggered();
	void on_actionPrev_triggered();
	void on_actionNext_triggered();
	
	bool openFile(QString fileName );
	bool openFiles(QStringList fileNames );


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
};

#endif
