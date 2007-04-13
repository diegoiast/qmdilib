#ifndef __HELP_PLUGIN_H__
#define __HELP_PLUGIN_H__

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
	
private:
	QAction *actionAbout;
	QAction *actionAboutQt;
	QAction *actionShowQtHelp;

	QString externalBrowser;
};

#endif// __HELP_PLUGIN_H__
