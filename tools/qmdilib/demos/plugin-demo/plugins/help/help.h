#ifndef __HELP_PLUGIN_H__
#define __HELP_PLUGIN_H__

#include "iplugin.h"

class QAction;

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
	
private:
	QAction *actionAbout;
	QAction *actionAboutQt;
	QAction *actionShowQtHelp;
};

#endif// __HELP_PLUGIN_H__
