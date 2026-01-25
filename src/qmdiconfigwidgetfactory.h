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

    // Data conversion
    virtual QVariant parse(const qmdiConfigItem &item, const QJsonValue &v) = 0;
    virtual QJsonValue serialize(const qmdiConfigItem &item, const QVariant &v) = 0;
};

class qmdiDefaultConfigWidgetFactory : public qmdiConfigWidgetFactory {
  public:
    QLabel *createLabel(const qmdiConfigItem &item, qmdiConfigDialog *parent) override;
    QWidget *createWidget(const qmdiConfigItem &item, qmdiConfigDialog *parent) override;
    QVariant getValue(QWidget *widget) override;
    void setValue(QWidget *widget, const QVariant &value) override;

    QVariant parse(const qmdiConfigItem &item, const QJsonValue &v) override;
    QJsonValue serialize(const qmdiConfigItem &item, const QVariant &v) override;
};

/**
 * @brief A type-safe wrapper for widget factories.
 *
 * Inherit from this class to create a factory that works with a concrete type T
 * instead of QVariant.
 */
template <typename T> class qmdiTypedConfigWidgetFactory : public qmdiDefaultConfigWidgetFactory {
  public:
    using ValueType = T;

    virtual QWidget *createWidget(const qmdiConfigItem &item, const T &initialValue,
                                  qmdiConfigDialog *parent) = 0;
    virtual T value(QWidget *widget) = 0;
    virtual void setValue(QWidget *widget, const T &value) = 0;

    // Data conversion (Typed) - Defaults to using the variant conversion system
    virtual T parseValue(const qmdiConfigItem &item, const QString &s) {
        Q_UNUSED(item);
        return fromVariant(s);
    }
    virtual QString serializeValue(const qmdiConfigItem &item, const T &v) {
        Q_UNUSED(item);
        return toVariant(v).toString();
    }

    // Conversion hooks: Default to Qt MetaType system if available, otherwise return default/empty.
    virtual T fromVariant(const QVariant &v) {
        if constexpr (QMetaTypeId2<T>::Defined) {
            return v.value<T>();
        }
        return T{};
    }

    virtual QVariant toVariant(const T &v) {
        if constexpr (QMetaTypeId2<T>::Defined) {
            return QVariant::fromValue(v);
        }
        return QVariant();
    }

    // Bridges...
    QWidget *createWidget(const qmdiConfigItem &item, qmdiConfigDialog *parent) override {
        return createWidget(item, fromVariant(item.value), parent);
    }

    QVariant getValue(QWidget *widget) override { return toVariant(value(widget)); }

    void setValue(QWidget *widget, const QVariant &value) override {
        setValue(widget, fromVariant(value));
    }

    QVariant parse(const qmdiConfigItem &item, const QJsonValue &v) override {
        return toVariant(parseValue(item, v.toString()));
    }

    QJsonValue serialize(const qmdiConfigItem &item, const QVariant &v) override {
        return serializeValue(item, fromVariant(v));
    }
};

class qmdiConfigWidgetRegistry {
  public:
    using FactoryCreator = std::function<std::unique_ptr<qmdiConfigWidgetFactory>()>;

    static qmdiConfigWidgetRegistry &instance();

    // Registration
    void registerFactory(qmdiConfigItem::ClassType type, FactoryCreator creator);
    void registerCustomFactory(const QString &customType, FactoryCreator creator);
    void clearCustomFactories();

    template <typename FactoryType> void registerCustomFactory(const QString &customType) {
        registerCustomFactory(customType, []() { return std::make_unique<FactoryType>(); });
    }

    // Dispatcher methods (The Registry acts as the Factory)
    QWidget *createWidget(const qmdiConfigItem &item, qmdiConfigDialog *parent);
    QLabel *createLabel(const qmdiConfigItem &item, qmdiConfigDialog *parent);
    QVariant getValue(const qmdiConfigItem &item, QWidget *widget);
    void setValue(const qmdiConfigItem &item, QWidget *widget, const QVariant &value);

    // Data Dispatcher methods
    QVariant parse(const qmdiConfigItem &item, const QJsonValue &v);
    QJsonValue serialize(const qmdiConfigItem &item, const QVariant &v);

    // Template Dispatcher methods for Non-MetaTypes
    template <typename T> QVariant toVariant(const QString &customType, const T &value) {
        qmdiConfigItem item;
        item.type = qmdiConfigItem::Custom;
        item.customTypeString = customType;

        auto handler = getHandler(item);
        if (auto typed = dynamic_cast<qmdiTypedConfigWidgetFactory<T> *>(handler.get())) {
            return typed->toVariant(value);
        }
        return QVariant();
    }

  private:
    qmdiConfigWidgetRegistry();
    std::unique_ptr<qmdiConfigWidgetFactory> getHandler(const qmdiConfigItem &item);

    void ensureDefaultFactoriesRegistered();
    bool defaultFactoriesRegistered = false;
    std::map<qmdiConfigItem::ClassType, FactoryCreator> factories;
    QMap<QString, FactoryCreator> customFactories;
};
