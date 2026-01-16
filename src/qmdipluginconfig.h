/**
 * \file qmdipluginconfig.h
 * \brief Definition of the plugin config
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#pragma once

#include <QJsonValue>
#include <QList>
#include <QString>
#include <QVariant>
#include <climits>

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
        Button,
        Label,
        Json,
        Custom,
        Last = INT_MAX
    };

    static ClassType typeFromString(const QString &typeStr);
    static QString typeToString(ClassType type);

    qmdiConfigItem();
    QString key;
    ClassType type;
    QString customTypeString;
    QString displayName;
    QString description;
    QVariant defaultValue;
    QVariant value;
    bool userEditable = true;
    bool forceShow = false;
    bool saveValue = true;
    QVariant possibleValue;

    struct Builder {
        Builder() : type(String), userEditable(true), forceShow(false) {}

        Builder &setKey(const QString &key);
        Builder &setType(const ClassType type);
        Builder &setCustomType(const QString &typeStr);
        Builder &setDisplayName(const QString &displayName);
        Builder &setDescription(const QString &description);
        Builder &setDefaultValue(const QVariant defaultValue);

        Builder &setDefaultValue(const char *val) {
            this->defaultValue = QString::fromUtf8(val);
            return *this;
        }

        template <typename T> Builder &setDefaultValue(const T &val) {
            this->defaultValue = QVariant::fromValue(val);
            return *this;
        }

        Builder &setValue(const QVariant &value);
        Builder &setUserEditable(const bool value);
        Builder &setPossibleValue(const QVariant &value);
        Builder &setForceShow(const bool value);

        qmdiConfigItem build() const;

      private:
        QString key;
        ClassType type;
        QString customTypeString;
        QString displayName;
        QString description;
        QVariant defaultValue;
        QVariant value;
        bool userEditable = true;
        bool forceShow = false;
        bool saveValue = true;
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

    QVariant getVariable(const QString &key) const {
        for (const auto &item : configItems) {
            if (item.key == key) {
                return !item.value.isNull() ? item.value : item.defaultValue;
            }
        }
        return {};
    }

    template <typename T> T getVariable(const QString &key) const {
        return getVariable(key).value<T>();
    }

    void setVariable(const QString &key, const QVariant &value) {
        for (auto &item : configItems) {
            if (item.key == key) {
                item.value = value;
                return;
            }
        }
    }

    template <typename T> void setVariable(const QString &key, T value) {
        setVariable(key, QVariant::fromValue(value));
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
