#ifndef __QMDI_WORKSPACE__
#define __QMDI_WORKSPACE__

/**
 * \file qmdiworkspace.h
 * \brief definition of the qmdi WorkSpace
 * \author Iastrubni <elcuco@kde.org>
 * Licence LGPL
 * \see qmdiWorkSpace
 */
 
// $Id$

#include <QWidget>
//#include <QWorkSpace>
#include "qmdiserver.h"

class QGridLayout;
class QWorkspace;
class QTabBar;
class qmdiHost;

class qmdiWorkspace : public QWidget, public qmdiServer
{
	Q_OBJECT
public:	
	qmdiWorkspace( QWidget *parent, qmdiHost *host=NULL );
	void addClient( qmdiClient *client );
	void addTab( QWidget *widget, QString name ); 
	QWidget *currentWidget();
	const QWidget * cornerWidget ( Qt::Corner corner = Qt::TopRightCorner );
	void setCornerWidget ( QWidget * widget, Qt::Corner corner = Qt::TopRightCorner );
	QWidget* widget( int i );
	int count();
	
public slots:	
	void workspaceChanged( QWidget * w );
	void tabBarChanged( int index );
	void windowDeleted( QObject *o );
	void tryCloseClient( int i );
	void tryCloseAllButClient( int i );
	void tryCloseAllCliens();
	void showClientMenu( int i, QPoint p );
	
private:
	QLayout		*mainLayout;
	QLayout		*headerLayout;	
	QTabBar		*tabBar;
	QWorkspace	*workspace;
	QWidgetList	_widgetList;
	
	QWidget		*cornerWidget1;
	QWidget		*cornerWidget2;
	QWidget		*activeWidget;
};
 	

#endif // __QMDI_WORKSPACE__
