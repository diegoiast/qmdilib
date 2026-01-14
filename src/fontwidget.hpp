/**
 * \file fontwidget.hpp
 * \brief implementation FontWidget
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#pragma once

#include <QLabel>

class FontWidget : public QLabel {
    Q_OBJECT
  public:
    FontWidget(QWidget *parent, const QString &fontStr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

  protected:
    void mousePressEvent(QMouseEvent *event) override;
};
