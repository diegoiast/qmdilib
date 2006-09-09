/**
 * \file qmdiworkspace.cpp
 * \brief implementation of qmdiWorkSpace
 * \author Diego Iastrubni <elcuco@kde.org>
 * Licence LGPL
 * \see qmdiWorkSpace
 */
 
// $Id$

#include <QWidget>
#include <QTabBar>
#include <QWorkspace>
#include <QVBoxLayout>
#include <QMainWindow>

#include "qmdiclient.h"
#include "qmdihost.h"
#include "qmdiworkspace.h"

qmdiWorkspace::qmdiWorkspace( QWidget *parent, qmdiHost *host )
	: QWidget( parent )
{
	if (host == NULL)
		mdiHost = dynamic_cast<qmdiHost*>(parent);
	else
		mdiHost = host;
		
	tabBar = new QTabBar;
	workspace = new QWorkspace;	
	mainLayout = new QVBoxLayout;
	
	tabBar->setDrawBase( false );	
	mainLayout->setMargin( 0 );
	mainLayout->setSpacing( 0 );
	mainLayout->addWidget(tabBar);
	mainLayout->addWidget(workspace);
	
	setLayout( mainLayout );
	connect( workspace, SIGNAL(windowActivated(QWidget*)), this, SLOT(workspaceChanged(QWidget*)));
	connect( tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabBarChanged(int)));
}

//qmdiWorkspace::~qmdiWorkspace()
//{
	//
//}

void qmdiWorkspace::addClient( qmdiClient *client )
{
	QWidget *w = dynamic_cast<QWidget*>(client);

	if (w == NULL)
		return;		
	
	addTab( w, client->name );
}

void qmdiWorkspace::addTab( QWidget *widget, QString name )
{
	if (!workspace)
		return;

	qmdiClient *client = dynamic_cast<qmdiClient*>(widget);
	if (client)
		client->mdiServer = this;
		
	connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(windowDeleted(QObject*)));
	widget->setAttribute(Qt::WA_DeleteOnClose, true);
	workspace->addWindow( widget );
	tabBar->addTab( name );
	widget->show();
}

QWidget *qmdiWorkspace::currentWidget()
{
	if (workspace)
		return workspace->activeWindow();
	else
		return NULL;
}

const QWidget * qmdiWorkspace::cornerWidget ( Qt::Corner corner  )
{
	return NULL;
}

void qmdiWorkspace::setCornerWidget ( QWidget * widget, Qt::Corner corner  )
{
	widget->setParent( tabBar );
	//tabBar->
}

void qmdiWorkspace::workspaceChanged( QWidget * w )
{
	static QWidget *activeWidget = NULL;
	static bool lock = false;
	
	if (lock)
		return;
	lock = true;
	if (activeWidget)
		mdiHost->unmergeClient( dynamic_cast<qmdiClient*>(activeWidget) );
	
	activeWidget = w;

	if (activeWidget)
		mdiHost->mergeClient( dynamic_cast<qmdiClient*>(activeWidget) );

	// try to update the mdi host
	QMainWindow *m = dynamic_cast<QMainWindow*>(mdiHost);
	mdiHost->updateGUI( m );
	
	// update the tab bar
	if (!workspace)
		return;
	int windowNumber = workspace->windowList().indexOf(w);
	if (windowNumber!=-1)
		tabBar->setCurrentIndex( windowNumber );

#if 0
	QWidgetList windowList = workspace->windowList();
	int windowCount = windowList.count();
	QString s;
	
	//qDebug("workspace chanegd (%d)", windowCount );
	for( int i=0; i<windowCount; i++ )		
	{
		//qDebug("removed tab (%d)", i );
		tabBar->removeTab( i );
	}
	
	//for (int i=0; i<windowCount; i++ )
	foreach( QWidget *w, windowList )
	{
		//QWidget *w = windowList[i];
		qmdiClient *c = dynamic_cast<qmdiClient*>(w);
		if (c)
		{
			s = c->name;
			if (s.isEmpty())
				s = c->fileName;
		}
		else
			s = w->objectName();

		//qDebug( "window %d -> %s", i, qPrintable(s) );		
		//if ((!s.isEmpty()) || (!s.isNull()))
		//tabBar->insertTab( i, s );
		tabBar->addTab( s );
	}
#endif
	lock = false;
}

void qmdiWorkspace::tabBarChanged( int index )
{
	if (!workspace)
			return;
	
	QWidget *newWindow = workspace->windowList()[index];
	if (!newWindow)
		return;
	newWindow->show();
	workspace->setActiveWindow ( newWindow );
	workspaceChanged( newWindow );
}

void qmdiWorkspace::windowDeleted( QObject *o )
{
	int windowNumber = workspace->windowList().lastIndexOf(dynamic_cast<QWidget*>(o));
	if (windowNumber!=-1)
		tabBar->removeTab( windowNumber );
}
