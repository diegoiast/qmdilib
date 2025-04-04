#pragma once

/**
 * \file qmdiclient.h
 * \brief Definition of the qmdi client class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiClient
 */

// the reason for including this file, and not declare the classes
// is for the developer using this library - one single include
#include "qmdiactiongroup.h"

#include "qmdiactiongrouplist.h"

class QString;
class qmdiServer;
class qmdiHost;

class qmdiClient {
  public:
    qmdiClient(const QString &newName = {});
    virtual ~qmdiClient();

    virtual bool closeClient();
    virtual bool canCloseClient();
    virtual QString mdiClientFileName();

    virtual void on_client_merged(qmdiHost *host);
    virtual void on_client_unmerged(qmdiHost *host);
    virtual std::optional<std::tuple<int, int, int>> get_coordinates() const;

    qmdiActionGroupList menus;
    qmdiActionGroupList toolbars;
    qmdiActionGroup contextMenu;
    qmdiServer *mdiServer = nullptr;
    QString mdiClientName;

    friend class qmdiHost;
};
