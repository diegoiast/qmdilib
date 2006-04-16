/**
 * \file qmdihost.cpp
 * \brief Implemetation of the qmdi host class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiHost
 */

#include <QMainWindow>
#include "qmdihost.h"
#include "qmdiclient.h"


qmdiHost::qmdiHost()
{
	toolBarList = NULL;
}

qmdiHost::~qmdiHost()
{
	delete toolBarList;
}


/**
 * \brief update the toolbars and menus
 * \param window the window to update
 * 
 * This function generates the menubar and toolbars from
 * the toolbar and menus definitions. 
 * 
 * You should call this method after every time you modify the menus
 * or structures.
 * 
 * The parameter window should be generally \b this , it's passed on
 * as a parameter, since qmdiHost cannot dynamic_cast it self to an 
 * QObject (this just does not work). On the other hand, this can give
 * you more freedom, as you do not have to derive the main window 
 * also from qmdiHost, and the host can be a separate object.
 * 
 * \see qmdiActionGroupList
 */
void qmdiHost::updateGUI( QMainWindow *window )
{
	if (window == NULL)
	{
		qDebug("%s - warning, no QMainWindow specified", __FUNCTION__ );
		return;
	}
		
	toolBarList = toolbars.updateToolBar( toolBarList, window );
	menus.updateMenu( window->menuBar() );
}

void qmdiHost::mergeClient( qmdiClient *client )
{
	if (client == NULL)
		return;
		
	menus.mergeGroupList( &client->menus );
	toolbars.mergeGroupList( &client->toolbars );
}

void qmdiHost::unmergeClient( qmdiClient *client )
{
	if (client == NULL)
		return;
		
	menus.unmergeGroupList( &client->menus );
	toolbars.unmergeGroupList( &client->toolbars );
}

