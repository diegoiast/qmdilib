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

#include <QObject>
#include "qmdiclient.h"

class QPopupMenu;
class qmdiHost;
class qmdiServer;
class IPlugin;

class PluginManager: public QObject, public qmdiClient
{
	Q_OBJECT
	
public:
	PluginManager( qmdiHost *host, qmdiServer *server );
	virtual ~PluginManager();

public slots:
	void addPlugin( IPlugin *newplugin );
	void removePlugin( IPlugin *oldplugin );
	
	void on_actionOpen_triggered();

private:
	QList<IPlugin*>		plugins;
	QAction			*actionOpen;
	QMenu			*newFilePopup;
};

#endif
