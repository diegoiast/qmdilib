#include <QAction>
#include <QIcon>

#include "iplugin.h"

// #define tr
QAction* new_action( QIcon icon, QString name, QObject *obj, QString shortcut, QString status_tip, const char*slot )
{
	QAction * a = new QAction( icon, name, obj );
	a->setShortcut(shortcut);
	a->setStatusTip(status_tip);
	a->connect( a, SIGNAL(triggered()), obj, slot );

	return a;
}


IPlugin::IPlugin()
{
	alwaysEnabled = false;
	autoEnabled = false;
	enabled = false;
}


IPlugin::~IPlugin()
{
}

void	IPlugin::showAbout()
{
}

QWidget*IPlugin::getConfigDialog()
{
	return NULL;
}

void	IPlugin::getData()
{
}

void	IPlugin::setData()
{
}

void	IPlugin::loadConfig( QSettings &settings )
{
}

void	IPlugin::SaveConfig( QSettings &settings )
{
}

bool 	IPlugin::isEnabled()
{
	return enabled;
}

QString	IPlugin::getName()
{
	return name;
}

QString IPlugin::getAuthor()
{
	return author;
}

QString IPlugin::getsVersion()
{
	return sVersion;
}

int	IPlugin::getiVersion()
{
	return iVersion;
}
