/**
 * \file qmditabbar.cpp
 * \brief Implementation of the qmdi tab bar widget
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiTabWidget, qmdiTabWidget, QTabWidget
 */

// $Id$

#include <QMouseEvent>
#include <QPoint>
#include "qmditabbar.h"

/**
 * \class qmdiTabBar
 * \brief Helper class for qmdiTabWidget
 * 
 * This class is a new tab bar, which emits some new signals
 * on mouse clicks. This helps some operations on the tab widget,
 * like recognizing right clicks and middle clicks.
 */

/**
 * \fn qmdiTabBar::middleMousePressed(int tabNumber, QPoint where );
 * \brief emitted when a user presses the middle mouse button [SIGNAL]
 * \param tabNumber the number of the tab pressed
 * \param where the location on the screen (relative) where the user clicked
 *
 * This is an emitted signal which happens when the middle mouse
 * on the tab bar
 */

/**
 * \fn qmdiTabBar::rightMousePressed(int tabNumber, QPoint where );
 * \brief emitted when a user presses the right mouse button [SIGNAL]
 * \param tabNumber the number of the tab pressed
 * \param where the location on the screen (relative) where the user clicked
 *
 * This is an emitted signal which happens when the right mouse
 * on the tab bar
 */

/**
 * \brief mouse pressed event handler
 * \param event
 *
 * This is an overloaded function, which will emit a signal
 * with the tab number which was pressed. There are 2 signals
 * emitted: middleMousePressed(int) and rightMousePressed(int)
 */
void qmdiTabBar::mousePressEvent ( QMouseEvent * event )
{
	if (event->button() == Qt::MidButton)
	{
		int tabCount = count();
		for( int i=0; i<tabCount; i++ )
		{
			if (tabRect(i).contains(event->pos()))
			{
				emit middleMousePressed(i, event->pos());
				break;
			}
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		int tabCount = count();
		for( int i=0; i<tabCount; i++ )
		{
			if (tabRect(i).contains(event->pos()))
			{
				emit rightMousePressed(i, event->pos());
				break;
			}
		}
	}

	QTabBar::mousePressEvent(event);
}
