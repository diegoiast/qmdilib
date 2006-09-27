#ifndef __QMDI_TAB_BAR_H__
#define __QMDI_TAB_BAR_H__

/**
 * \file qmditabbar.h
 * \brief Declaration of the qmdi tab bar widget
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiTabWidget, qmdiTabWidget, QTabWidget
 */
 
// $Id$ 
 
#include <QTabBar>

class QMouseEvenet;
class QPoint;

class qmdiTabBar: public QTabBar
{
	Q_OBJECT

	protected:
		void mousePressEvent ( QMouseEvent * event );
		
	signals:
		void middleMousePressed( int tabNumber, QPoint where );
		void rightMousePressed( int tabNumber, QPoint where );
};

#endif // __QMDI_TAB_BAR_H__
