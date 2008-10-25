#ifndef __QMDI_HOST_H__
#define __QMDI_HOST_H__


/**
 * \file qmdihost.h
 * \brief Definition of the qmdi host class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiHost
 */
 
// $Id$ 

// the reason for including this file, and not declare the classes
// is for the developer using this library - one single include
#include "qmdiactiongroup.h"

#include "qmdiactiongrouplist.h"


class QMainWindow;
class qmdiClient;

class qmdiHost
{
public:
	qmdiHost();
	virtual ~qmdiHost();
	
	qmdiActionGroupList menus;
	qmdiActionGroupList toolbars;

	void updateGUI( QMainWindow *window = NULL );
	void mergeClient( qmdiClient *client );
	void unmergeClient( qmdiClient *client );
	
	QList<QToolBar*>* toolBarList;
protected:
	void addActionsToWidget( qmdiActionGroupList &agl, QWidget *w );
	void removeActionsFromWidget( qmdiActionGroupList &agl, QWidget *w );
	bool updateMenusAndToolBars;
};

#endif // __QMDI_CLIENT_H__
