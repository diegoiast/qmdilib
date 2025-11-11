#pragma once

/**
 * \file qmdiclient.h
 * \brief Definition of the qmdiClient class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiServer
 */

// the reason for including this file, and not declare the classes
// is for the developer using this library - one single include
#include "qmdiactiongroup.h"
#include "qmdiactiongrouplist.h"

#include <QHash>

class qmdiServer;
class qmdiHost;

using qmdiClientState = QHash<QString, QVariant>;

enum class CloseReason { ApplicationQuit, CloseTab };

class qmdiClient {
  public:
    qmdiClient(const QString &newName = {});
    virtual ~qmdiClient();

    virtual bool closeClient(CloseReason reason);
    virtual bool canCloseClient(CloseReason reason);
    virtual QString mdiClientFileName();

    virtual void on_client_merged(qmdiHost *host);
    virtual void on_client_unmerged(qmdiHost *host);
    virtual std::optional<std::tuple<int, int, int>> get_coordinates() const;

    virtual qmdiClientState getState() const;
    virtual void setState(const qmdiClientState &state);

    qmdiActionGroupList menus;
    qmdiActionGroupList toolbars;
    qmdiActionGroup contextMenu;
    qmdiServer *mdiServer = nullptr;
    QString mdiClientName;
};
