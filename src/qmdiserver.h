#pragma once

/**
 * \file qmdiserver.h
 * \brief Definition of the qmdi server class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiServer
 */

#include <functional>

class QPoint;
class qmdiClient;
class qmdiHost;

class qmdiServer {
  public:
    qmdiServer();
    virtual ~qmdiServer();
    virtual void addClient(qmdiClient *client) = 0;
    virtual void deleteClient(qmdiClient *) = 0;
    virtual int getClientsCount() const = 0;
    virtual qmdiClient *getClient(int i) const = 0;
    virtual qmdiClient *getCurrentClient() const = 0;
    virtual void setCurrentClientIndex(int i) = 0;
    virtual int getCurrentClientIndex() const = 0;

    void tryCloseClient(int i);
    void tryCloseAllButClient(int i);
    void tryCloseAllClients();
    void showClientMenu(int i, QPoint p);
    void setOnMdiSelected(std::function<void(qmdiClient *, int)> &&callback) {
        onMdiSelected = std::move(callback);
    }
    virtual void mdiSelected(qmdiClient *client, int index) const = 0;

    qmdiHost *mdiHost;

  protected:
    std::function<void(qmdiClient *, int)> onMdiSelected;
};
