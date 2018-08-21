#ifndef __HELP_PLUGIN_H__
#define __HELP_PLUGIN_H__

/**
 * \file help_plg.h
 * \brief Definition of the HelpPlugin class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL
 * \see HelpPlugin
 */

// $Id$

#include "iplugin.h"

class QAction;
class QUrl;

class HelpPlugin: public IPlugin
{
	Q_OBJECT
public:
	HelpPlugin();
	~HelpPlugin();

public slots:
	void showAbout();
	void showAboutApp();
	void showAboutQt();
	void showQtHelp();

	void on_browser_sourceChanged ( const QUrl & src );
	
	int canOpenFile( const QString fileName );
	bool openFile( const QString fileName, int x=-1, int y=-1, int z=-1 );
	bool loadHTML( QString fileName, int x=-1, int y=-1, int z=-1  );
	
private:
	QAction *actionAbout;
	QAction *actionAboutQt;
	QAction *actionShowQtHelp;

	QString externalBrowser;
};

#endif// __HELP_PLUGIN_H__
