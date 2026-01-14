/**
 * \file fontwidget.hpp
 * \brief implementation FontWidget
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#include <QFontDialog>
#include <QMouseEvent>
#include <QApplication>

#include "fontwidget.hpp"

FontWidget::FontWidget(QWidget* parent, const QString& fontStr)
    : QLabel(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setCursor(Qt::PointingHandCursor);

    if (!fontStr.isEmpty()) {
        QFont f;
        f.fromString(fontStr);
        setFont(f);
        setText(fontStr);
    }
}

QSize FontWidget::sizeHint() const
{
    QFont f = QApplication::font();
    if (parentWidget()) {
        f = parentWidget()->font();
    }

    QFontMetrics fm(f);
    QSize s = fm.size(0, text());
    int m = 4;
    return QSize(s.width() + m * 2, std::max(s.height(), fm.lineSpacing()) + m * 2);
}

QSize FontWidget::minimumSizeHint() const
{
    QSize s = sizeHint();
    return QSize(1, s.height());
}

void FontWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, font(), this);
    if (ok) {
        setFont(newFont);
        setText(newFont.toString());
    }
    QLabel::mousePressEvent(event);
}

