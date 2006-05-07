#ifndef __QMDI_TAB_WIDGET_H__
#define __QMDI_TAB_WIDGET_H__

/**
 * \file qmditabwidget.h
 * \brief Declaration of the qmdi tab widget
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiServer, QTabWidget
 */
 
#include <QTabWidget>
#include <QTabBar>
#include "qmdiserver.h"

class QWidget;
class QPoint;
class qmdiHost;

class qmdiTabBar: public QTabBar
{
	Q_OBJECT

	protected:
		void mousePressEvent ( QMouseEvent * event );
		
	signals:
		void middleMousePressed(int tabNumber, QPoint where );
		void rightMousePressed(int tabNumber, QPoint where );
};

class qmdiTabWidget : public QTabWidget, public qmdiServer
{
	Q_OBJECT
public:
	qmdiTabWidget( QWidget *parent=NULL, qmdiHost *host=NULL );

public slots:
	void tabChanged( int i );
	void tryCloseClient( int i );
	void tryCloseAllButClient( int i );
	void tryCloseAllCliens();
	void showClientMenu( int i, QPoint p );

protected:
	void clientDeleted( QObject *o );
	void tabInserted ( int index );
	void tabRemoved ( int index );
	void addClient( qmdiClient *client );
	
private:
	QWidget		*activeWidget;
	qmdiHost	*mdiHost;
};

#endif // __QMDI_TAB_WIDGET_H__
