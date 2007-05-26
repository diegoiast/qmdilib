/**
 * \file iplugin.cpp
 * \brief Implementation of the IPlugin interface
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL
 * \see IPlugin
 */

// $Id$

#include <QAction>
#include <QActionGroup>
#include <QIcon>

#include "iplugin.h"

/**
 * \class IPlugin
 * \brief an abstract interface for describing Plugins
 * 
 * 
 * 
 * 
 */

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

void	IPlugin::loadConfig( const QSettings &settings )
{
}

void	IPlugin::SaveConfig( const QSettings &settings )
{
}

QActionGroup*	IPlugin::newFileActions()
{
	// by default plugins cannot create any files
	return NULL;
}

QStringList	IPlugin::myExtensions()
{
	QStringList s;

	// no available extensions by default
	return s;
}

int 	IPlugin::canOpenFile( const QString fileName )
{
	// can't open no file
	return -1;
}

bool	IPlugin::openFile( QString fileName, int x, int y, int z )
{
	// hacks to avoid warnings
	x = y = z = fileName.length();
	
	// refuse to open any file
	return false;
}

bool 	IPlugin::isEnabled()
{
	return enabled;
}

void	IPlugin::setEnabled( bool b )
{
	b = enabled;
}

bool 	IPlugin::canDisable()
{
	return	!alwaysEnabled;
}

QIcon	IPlugin::getIcon()
{
	return QIcon("images/config.png");
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
