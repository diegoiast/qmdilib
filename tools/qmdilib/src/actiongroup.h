#ifndef __ACTION_GROUP_H__
#define __ACTION_GROUP_H__

/**
 * \file actiongroup.h
 * \brief Definition of the action group class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiActionGroup
 */
 
#include <QList>
#include <QString>

class QAction;
class QObject;
class QWidget;
class QMenu;
class QToolBar;

class qmdiActionGroup
{
public:
	qmdiActionGroup( QString name );
	qmdiActionGroup();
	~qmdiActionGroup();

	void		setName( QString name );
	QString		getName();	
	void		addAction( QAction *action );
	void		addWidget( QWidget *widget );
	void		addSeparator();
	bool		containsAction( QAction *action );
	void		removeAction( QAction *action );
	void		removeWidget( QWidget *widget );

	void		mergeGroup( qmdiActionGroup *group );
	void		unmergeGroup( qmdiActionGroup *group );

	QMenu*		updateMenu( QMenu *menu=NULL );
	QToolBar*	updateToolBar( QToolBar *toolbar );

	bool		breakAfter;
private:
	QString name;
	QList<QObject*> actionGroupItems;

	int breakCount;
};

#endif //__ACTION_GROUP__
