#pragma once

/**
 * \file qmditabwidget.h
 * \brief Declaration of the qmdi tab widget
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiServer, QTabWidget
 */

#include <QTabWidget>

#include "qmdiserver.h"

class QWidget;
class QPoint;
class QEvent;

class qmdiHost;

class qmdiTabWidget : public QTabWidget, public qmdiServer {
    Q_OBJECT
  public:
    explicit qmdiTabWidget(QWidget *parent = nullptr, qmdiHost *host = nullptr);
    ~qmdiTabWidget() override = default;

  public slots:
    void tabChanged(int i);
    void on_middleMouse_pressed(int, QPoint);
    void on_rightMouse_pressed(int, QPoint);

  signals:
    void newClientAdded(qmdiClient *);

  public:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void addClient(qmdiClient *client) override;
    virtual void deleteClient(qmdiClient *client) override;
    virtual int getClientsCount() const override;

    virtual qmdiClient *getClient(int i) const override;
    virtual qmdiClient *getCurrentClient() const override;
    virtual void setCurrentClientIndex(int i) override;
    virtual int getCurrentClientIndex() const override;

  protected:
    virtual void tabInserted(int index) override;
    virtual void tabRemoved(int index) override;
    virtual void mdiSelected(qmdiClient *client, int index) const override;

  private:
    QWidget *activeWidget;
};
