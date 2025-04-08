/**
 * \file qmdipluginconfig.h
 * \brief Definition of the plugin config
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#pragma once

#include <QList>
#include <QString>
#include <QVariant>

struct qmdiConfigItem {
    enum ClassType {
        String,
        Bool,
        Int8,
        Int16,
        Int32,
        UInt8,
        UInt16,
        UInt32,
        Float,
        Double,
        StringList,
        OneOf,
        Font,
        Path,
        Last = 100000
    };

    static ClassType typeFromString(const QString &typeStr);
    static QString typeToString(ClassType type);

    qmdiConfigItem();
    QString key;
    ClassType type;
    QString displayName;
    QString description;
    QVariant defaultValue;
    QVariant value;
    bool userEditable;
    QVariant possibleValue;

    struct Builder {
        Builder() : type(String), userEditable(true) {}

        Builder &setKey(const QString &key);
        Builder &setType(const ClassType type);
        Builder &setDisplayName(const QString &displayName);
        Builder &setDescription(const QString &description);
        Builder &setDefaultValue(const QVariant defaultValue);
        Builder &setValue(const QVariant &value);
        Builder &setUserEditable(const bool value);
        Builder &setPossibleValue(const QVariant &value);

        qmdiConfigItem build() const;

      private:
        QString key;
        ClassType type;
        QString displayName;
        QString description;
        QVariant defaultValue;
        QVariant value;
        bool userEditable = true;
        QVariant possibleValue;
    };
    void setDefault();
};

class qmdiPluginConfig {
  public:
    QString pluginName;
    QString description;
    QList<qmdiConfigItem> configItems;

    qmdiPluginConfig() = default;
    void setDefault();
    int editableConfigs() const;

    template <typename T> void setVariable(const QString &key, T value) {
        for (auto &item : configItems) {
            if (item.key != key) {
                continue;
            }
            item.value = value;
            return;
        }
    }

    template <typename T> T getVariable(const QString &key) const {
        for (const auto &item : configItems) {
            if (item.key == key) {
                if (!item.value.isNull()) {
                    return item.value.value<T>();
                } else {
                    return item.defaultValue.value<T>();
                }
            }
        }
        return {};
    }

    class Builder {
      public:
        Builder() = default;

        Builder &setPluginName(const QString &name) {
            pluginName = name;
            return *this;
        }

        Builder &setDescription(const QString &desc) {
            description = desc;
            return *this;
        }

        Builder &addConfigItem(const qmdiConfigItem &item) {
            configItems.append(item);
            return *this;
        }

        qmdiPluginConfig build() const {
            qmdiPluginConfig pluginConfig;
            pluginConfig.pluginName = this->pluginName;
            pluginConfig.description = this->description;
            pluginConfig.configItems = this->configItems;
            return pluginConfig;
        }

      private:
        QString pluginName;
        QString description;
        QList<qmdiConfigItem> configItems;
    };
};
