#pragma once

/**
 * \file qmdiactiongrouplist.h
 * \brief Definition of the action group list class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiActionGroupList
 */

#include <QList>

class QWidget;
class QToolBar;
class QObject;
class QAction;
class QString;
class QMenuBar;
class QMenu;
class QMainWindow;

class qmdiActionGroup;

class qmdiActionGroupList {
    friend class qmdiHost;

  public:
    qmdiActionGroupList();
    ~qmdiActionGroupList();

    qmdiActionGroup *operator[](const QString &name);
    qmdiActionGroup *getActionGroup(const QString &name);
    qmdiActionGroup *addActionGroup(const QString &name, const QString &after);
    void mergeGroupList(qmdiActionGroupList *group);
    void unmergeGroupList(qmdiActionGroupList *group);

    QMenuBar *updateMenuBar(QMenuBar *menubar);
    QMenu *updatePopMenu(QMenu *popupMenu);
    QList<QToolBar *> *updateToolBar(QList<QToolBar *> *toolbars, QMainWindow *window);

    void addActionsToWidget(QWidget *widget);
    void removeActionsFromWidget(QWidget *widget);
    int size() const { return actionGroups.size(); }

  private:
    QList<qmdiActionGroup *> actionGroups;
};
