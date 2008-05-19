#ifndef __ACTION_GROUP_LIST_H__
#define __ACTION_GROUP_LIST_H__

/**
 * \file qmdiactiongrouplist.h
 * \brief Definition of the action group list class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiActionGroupList
 */
 
// $Id$

#include <QList>
#include <QToolBar>
//#include "qmdiactiongroup.h"

class QObject;
class QAction;
class QString;
class QMenuBar;
class QMainWindow;

class qmdiActionGroup;

class qmdiActionGroupList
{
public:
	qmdiActionGroupList();
	
	qmdiActionGroup* operator[]( const QString name );
	qmdiActionGroup* getActionGroup( const QString name );
	void mergeGroupList( qmdiActionGroupList *group );
	void unmergeGroupList( qmdiActionGroupList *group );
	
	QMenuBar*		updateMenu( QMenuBar *menubar );
	QList<QToolBar*>*	updateToolBar( QList<QToolBar*> *toolbars, QMainWindow *window );
	
private:
	QList<qmdiActionGroup*> actionGroups;
};

#endif //__ACTION_GROUP__
