#ifndef __QMDI_TAB_WIDGET_H__
#define __QMDI_TAB_WIDGET_H__

/**
 * \file qmditabwidget.h
 * \brief Declaration of the qmdi tab widget
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiServer, QTabWidget
 */
 
// $Id$ 
 
#include <QTabWidget>
#include <QTabBar>
#include "qmdiserver.h"

class QWidget;
class QPoint;
class QEvent;

class qmdiHost;

class qmdiTabWidget : public QTabWidget, public qmdiServer
{
	Q_OBJECT
public:
	qmdiTabWidget( QWidget *parent=NULL, qmdiHost *host=NULL );
	~qmdiTabWidget();

public slots:
	void tabChanged( int i );
	void wSpaceWindowActivated( QWidget* w );
	void on_middleMouse_pressed( int, QPoint );
	void on_rightMouse_pressed( int, QPoint );
	bool eventFilter(QObject *obj, QEvent *event);
	
	// need to overide this functions
public:
	virtual void addClient( qmdiClient *client  );
	virtual void deleteClient( qmdiClient* client );
	virtual int getClientsCount();
	virtual qmdiClient *getClient( int i );

protected:
	void tabInserted ( int index );
	void tabRemoved ( int index );
	
private:
	QWidget		*activeWidget;
};

#endif // __QMDI_TAB_WIDGET_H__
