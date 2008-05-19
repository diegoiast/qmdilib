#ifndef __ACTION_GROUP_H__
#define __ACTION_GROUP_H__

/**
 * \file qmdiactiongroup.h
 * \brief Definition of the action group class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiActionGroup
 */
 
// $Id$ 
 
#include <QList>
#include <QString>

class QAction;
class QObject;
class QWidget;
class QMenu;
class QToolBar;
class QActionGroup;

class qmdiActionGroup
{
public:
	qmdiActionGroup( QString name );
	~qmdiActionGroup();

	void		setName( QString name );
	QString		getName();	
	void		addAction( QAction *action, int location=-1 );
	void		addActions( QActionGroup *actions, int location=-1 );
	void		addWidget( QWidget *widget, int location=-1 );
	void		addMenu( QMenu *menu, int location=-1 );
	void		addSeparator( int location=-1 );
	bool		containsAction( QAction *action );
	void		removeAction( QAction *action );
	void		removeActions( QActionGroup *actions );
	void		removeMenu( QMenu *menu );
	void		removeWidget( QWidget *widget );
	void		setMergePoint();
	int		getMergePoint();

	void		mergeGroup( qmdiActionGroup *group );
	void		unmergeGroup( qmdiActionGroup *group );

	QMenu*		updateMenu( QMenu *menu=NULL );
	QToolBar*	updateToolBar( QToolBar *toolbar );

	bool		breakAfter;
private:
	QString name;
	QList<QObject*> actionGroupItems;
	QList<qmdiActionGroup*> actionGroups;

	int breakCount;
	int mergeLocation;
};

#endif //__ACTION_GROUP__
