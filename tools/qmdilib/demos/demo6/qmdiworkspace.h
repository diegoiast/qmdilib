#ifndef __QMDI_WORKSPACE__
#define __QMDI_WORKSPACE__

/**
 * \file qmdiworkspace.cpp
 * \brief definition of qmdiWorkSpace
 * \author Iastrubni <elcuco@kde.org>
 * Licence LGPL
 * \see qmdiWorkSpace
 */
 
// $Id$

#include <QWidget>
//#include <QWorkSpace>
#include "qmdiserver.h"

class QLayout;
class QWorkspace;
class QTabBar;
class qmdiHost;

class qmdiWorkspace : public QWidget, public qmdiServer
{
	Q_OBJECT
public:	
	qmdiWorkspace( QWidget *parent, qmdiHost *host=NULL );
	//~qmdiWorkspace();
	void addClient( qmdiClient *client );
	void addTab( QWidget *widget, QString name ); 
	QWidget *currentWidget();
	const QWidget * cornerWidget ( Qt::Corner corner = Qt::TopRightCorner );
	void setCornerWidget ( QWidget * widget, Qt::Corner corner = Qt::TopRightCorner );
	
public slots:	
	void workspaceChanged( QWidget * w );
	void tabBarChanged( int index );
	void windowDeleted( QObject *o );
	
private:
	qmdiHost	*mdiHost;
	QLayout		*mainLayout;	
	QTabBar		*tabBar;
	QWorkspace	*workspace;
};
 	

#endif // __QMDI_WORKSPACE__
