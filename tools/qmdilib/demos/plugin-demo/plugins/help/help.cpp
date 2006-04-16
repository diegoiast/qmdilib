#include <QAction>
#include <QApplication>
#include <QUrl>
#include <QLibraryInfo>
#include <QMessageBox>

#include "iplugin.h"
#include "qmdiserver.h"
#include "help.h"
#include "helpbrowse.h"


HelpPlugin::HelpPlugin()
	: IPlugin()
{
	actionAbout	= new_action( QIcon(), tr("&About"), this, "", tr("XXXXX"), SLOT(showAboutApp()) );
	actionAboutQt	= new_action( QIcon(), tr("&About Qt"), this, "", tr("XXXXX"), SLOT(showAboutQt()) );
 	actionShowQtHelp= new_action( QIcon(), tr("&Qt help"), this, "", tr("XXXXX"), SLOT(showQtHelp()) );

	name = "Help plugin";
	author = "Diego Iastrubni <elcuco@kde.org>";
	iVersion = 0;
	sVersion = "0.0.1";
	autoEnabled = true;
	alwaysEnabled = true;

	menus["&Help"]->addAction( actionShowQtHelp );
	menus["&Help"]->addSeparator();
	menus["&Help"]->addAction( actionAbout );
	menus["&Help"]->addAction( actionAboutQt );
}

HelpPlugin::~HelpPlugin()
{
	delete actionAbout;
	delete actionAboutQt;
	delete actionShowQtHelp;
}

void HelpPlugin::showAbout()
{
}

void HelpPlugin::showAboutApp()
{
	QMessageBox::about( 0, "qmdilib demo", "A small demo for loading plugins<br>Diego Iastrubni (elcuco@kde.org) - lgpl)" );
}

void HelpPlugin::showAboutQt()
{
	QApplication::aboutQt();
}

void HelpPlugin::showQtHelp()
{
	QString helpFile = QLibraryInfo::location(QLibraryInfo::DocumentationPath) + QLatin1String("/html/index.html");
	QexHelpBrowser *browser = new QexHelpBrowser( QUrl(helpFile) );
	browser->hide();
	browser->name = "Qt help";
	browser->setObjectName( browser->name );
	mdiServer->addClient( browser );
}
