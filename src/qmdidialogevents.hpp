/**
 * \file qmdiglobalevents.h
 * \brief Global event system for qmdilib
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 */

#pragma once

#include "qmdiconfigdialog.h"
#include <QObject>
#include <QString>

class qmdiDialogEvents : public QObject {
    Q_OBJECT
  public:
    static qmdiDialogEvents &instance() {
        static qmdiDialogEvents instance;
        return instance;
    }

    // Prevent copying
    qmdiDialogEvents(const qmdiDialogEvents &) = delete;
    qmdiDialogEvents &operator=(const qmdiDialogEvents &) = delete;

  signals:
    void buttonClicked(qmdiConfigDialog *dialog, const QString &buttonKey);
    void linkClicked(qmdiConfigDialog *dialog, const QString &labelKey, const QString &linkUrl);

  private:
    qmdiDialogEvents() {}
};
