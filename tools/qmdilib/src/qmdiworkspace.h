#ifndef __QMDI_WORKSPACE__
#define __QMDI_WORKSPACE__

/**
 * \file qmdiworkspace.h
 * \brief definition of the qmdi Workspace
 * \author Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiServer
 */
 
// $Id$

#include <QWidget>
#include "qmdiserver.h"

class QGridLayout;
class QWorkspace;
class QTabBar;
class qmdiHost;

class qmdiWorkspace : public QWidget, public qmdiServer
{
	Q_OBJECT
public:	
	qmdiWorkspace( QWidget *parent=NULL, qmdiHost *host=NULL );
	
	// compability with QTabWidget
	void			addTab( QWidget *widget, QString name ); 
	QWidget*		currentWidget();
	const QWidget*		cornerWidget ( Qt::Corner corner = Qt::TopRightCorner );
	void			setCornerWidget ( QWidget * widget, Qt::Corner corner = Qt::TopRightCorner );
	QWidget*		widget( int i );
	int			currentIndex();
	int			count();
	
	// overloaded methods
	virtual void		addClient( qmdiClient *client );
	virtual int		getClientsCount();
	virtual qmdiClient	*getClient( int i );
	
public slots:	
	bool eventFilter( QObject *obj, QEvent *event );
	void workspaceChanged( QWidget * w );
	void tabBarChanged( int index );
	void windowDeleted( QObject *o );
	void on_middleMouse_pressed( int, QPoint );
	void on_rightMouse_pressed( int, QPoint );
	
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
