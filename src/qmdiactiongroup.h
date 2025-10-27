#pragma once

/**
 * \file qmdiactiongroup.h
 * \brief Definition of the action group class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiActionGroup
 */

#include <QList>

class QAction;
class QActionGroup;
class QMenu;
class QToolBar;
class QString;
class QObject;
class QWidget;

class qmdiActionGroup {
    friend class qmdiHost;

  public:
    explicit qmdiActionGroup(const QString &name);
    qmdiActionGroup();
    ~qmdiActionGroup();

    void setName(const QString &newName);
    bool empty() const { return actionGroupItems.empty() && actionGroups.empty(); }
    QString getName() const;
    void addAction(QAction *action, int location = -1);
    void addActions(QActionGroup *actions, int location = -1);
    void addWidget(QWidget *widget, int location = -1);
    void addMenu(QMenu *menu, int location = -1);
    void addSeparator(int location = -1);
    bool containsAction(const QAction *action) const;
    void removeAction(const QAction *action);
    void removeActions(QActionGroup *actions);
    void removeMenu(const QMenu *menu);
    void removeWidget(const QWidget *widget);
    void setMergePoint();
    int getMergePoint() const;
    QAction *findActionNamed(const QString &);

    void mergeGroup(qmdiActionGroup *group);
    void unmergeGroup(const qmdiActionGroup *group);

    QMenu *updateMenu(QMenu *menu = nullptr, bool needeEmptyIcon = false) const;
    QToolBar *updateToolBar(QToolBar *toolbar) const;
    void addActionsToWidget(QWidget *widget);
    void removeActionsFromWidget(QWidget *widget);

    bool breakAfter;

  private:
    QString name;
    QList<QObject *> actionGroupItems;
    QList<qmdiActionGroup *> actionGroups;

    int breakCount;
    int mergeLocation;
};
