/**
 * \file qmdiconfigwidgetfactory.h
 * \brief Widget factory definition
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 */

#pragma once

#include "qmdipluginconfig.h"
#include <QLabel>
#include <QWidget>
#include <functional>
#include <memory>

#include "stringlistwidget.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

class qmdiConfigDialog;

class qmdiConfigWidgetFactory {
  public:
    virtual ~qmdiConfigWidgetFactory() = default;
    virtual QWidget *createWidget(const qmdiConfigItem &item, qmdiConfigDialog *parent) = 0;
    virtual QLabel *createLabel(const qmdiConfigItem &item, qmdiConfigDialog *parent) = 0;
    virtual QVariant getValue(QWidget *widget) = 0;
    virtual void setValue(QWidget *widget, const QVariant &value) = 0;
};

class qmdiDefaultConfigWidgetFactory : public qmdiConfigWidgetFactory {
  public:
    QWidget *createWidget(const qmdiConfigItem &item, qmdiConfigDialog *parent) override;
    QLabel *createLabel(const qmdiConfigItem &item, qmdiConfigDialog *parent) override;
    QVariant getValue(QWidget *widget) override;
    void setValue(QWidget *widget, const QVariant &value) override;
};

class qmdiConfigWidgetRegistry {
  public:
    using FactoryCreator = std::function<std::unique_ptr<qmdiConfigWidgetFactory>()>;

    static qmdiConfigWidgetRegistry &instance();
    void registerFactory(qmdiConfigItem::ClassType type, FactoryCreator creator);
    std::unique_ptr<qmdiConfigWidgetFactory> createFactory(qmdiConfigItem::ClassType type);

  private:
    qmdiConfigWidgetRegistry();

    void ensureDefaultFactoriesRegistered();
    bool defaultFactoriesRegistered = false;
    std::map<qmdiConfigItem::ClassType, FactoryCreator> factories;
};
