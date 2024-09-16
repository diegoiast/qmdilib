/**
 * \file qmdipluginconfig.h
 * \brief Implementation of the global config
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#include "qmdiglobalconfig.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

qmdiGlobalConfig::qmdiGlobalConfig(QObject *parent) : QObject(parent) {}

void qmdiGlobalConfig::setDefaults() {
    for (auto it = pluginMap.begin(); it != pluginMap.end(); ++it) {
        qmdiPluginConfig *pluginConfig = it.value();
        for (auto &config : pluginConfig->configItems) {
            config.setDefault();
        }
    }
}

bool qmdiGlobalConfig::loadDefsFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open file" << filePath << "for reading.";
        return false;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    QJsonObject jsonObject = jsonDoc.object();
    return loadDefsFromJson(jsonObject);
}

bool qmdiGlobalConfig::loadDefsFromJson(const QJsonObject &jsonObject) {
    QJsonArray pluginsArray = jsonObject["plugins"].toArray();

    qDeleteAll(plugins);
    plugins.clear();
    pluginMap.clear();
    for (const QJsonValue &value : std::as_const(pluginsArray)) {
        QJsonObject pluginObject = value.toObject();
        qmdiPluginConfig *pluginConfig = new qmdiPluginConfig();

        pluginConfig->pluginName = pluginObject["pluginName"].toString();
        pluginConfig->description = pluginObject["description"].toString();

        QJsonArray configItemsArray = pluginObject["configItems"].toArray();
        pluginConfig->configItems.clear();

        for (const QJsonValue &itemValue : std::as_const(configItemsArray)) {
            QJsonObject itemObject = itemValue.toObject();
            qmdiConfigItem item;

            item.key = itemObject["key"].toString();
            item.type = qmdiConfigItem::typeFromString(itemObject["type"].toString());
            item.displayName = itemObject["displayName"].toString();
            item.description = itemObject["description"].toString();

            QJsonValue defaultValue = itemObject["defaultValue"];
            switch (item.type) {
            case qmdiConfigItem::String:
                item.defaultValue = defaultValue.toString();
                break;
            case qmdiConfigItem::Bool:
                item.defaultValue = defaultValue.toBool();
                break;
            case qmdiConfigItem::Int8:
                item.defaultValue = defaultValue.toInt();
                break;
            case qmdiConfigItem::Int16:
                item.defaultValue = defaultValue.toInt();
                break;
            case qmdiConfigItem::Int32:
                item.defaultValue = defaultValue.toInt();
                break;
            case qmdiConfigItem::UInt8:
                item.defaultValue = defaultValue.toInt();
                break;
            case qmdiConfigItem::UInt16:
                item.defaultValue = defaultValue.toInt();
                break;
            case qmdiConfigItem::UInt32:
                item.defaultValue = defaultValue.toInt();
                break;
            case qmdiConfigItem::Float:
                item.defaultValue = defaultValue.toDouble();
                break;
            case qmdiConfigItem::Double:
                item.defaultValue = defaultValue.toDouble();
                break;
            case qmdiConfigItem::StringList:
                item.defaultValue = defaultValue.toVariant().toStringList();
                break;
            }

            pluginConfig->configItems.append(item);
        }

        pluginMap[pluginConfig->pluginName] = pluginConfig;
        plugins.append(pluginConfig);
    }

    return true;
}

bool qmdiGlobalConfig::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    QByteArray fileData = file.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(fileData));
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Failed to parse JSON or JSON is not an object.";
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();
    fromJson(jsonObj);
    return true;
}

bool qmdiGlobalConfig::saveToFile(const QString &filePath) {
    QJsonObject jsonObject = asJson();
    QJsonDocument jsonDoc(jsonObject);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error opening file for writing:" << file.errorString();
        return false;
    }

    file.write(jsonDoc.toJson());
    file.close();
    return true;
}

QJsonObject qmdiGlobalConfig::asJson() const {
    QJsonObject jsonObject;

    for (auto it = pluginMap.begin(); it != pluginMap.end(); ++it) {
        const qmdiPluginConfig *pluginConfig = it.value();
        QJsonObject pluginObject;
        QJsonArray configItemsArray;
        for (const qmdiConfigItem &item : std::as_const(pluginConfig->configItems)) {
            QJsonObject itemObject;
            itemObject["key"] = item.key;

            if (item.value.typeId() == QMetaType::QStringList) {
                QStringList stringList = item.value.value<QStringList>();
                QJsonArray jsonArray;
                for (const QString &str : std::as_const(stringList)) {
                    jsonArray.append(str);
                }
                itemObject["value"] = jsonArray;
            } else {
                itemObject["value"] = item.value.toString();
            }
            configItemsArray.append(itemObject);
        }
        pluginObject["configItems"] = configItemsArray;
        jsonObject[pluginConfig->pluginName] = pluginObject;
    }

    return jsonObject;
}

void qmdiGlobalConfig::fromJson(QJsonObject jsonObj) {
    auto findKey = [](auto array, auto key) -> QJsonObject {
        for (const QJsonValue &itemValue : array) {
            if (!itemValue.isObject()) {
                continue;
            }
            const QJsonObject configItemObj = itemValue.toObject();
            if (configItemObj.contains("key") && configItemObj["key"].isString()) {
                const QString k = configItemObj["key"].toString();
                if (k == key) {
                    return configItemObj;
                }
            }
        }
        return {};
    };

    for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it) {
        const QString pluginName = it.key();
        const QJsonObject pluginObj = it.value().toObject();

        auto pluginConfig = pluginMap.value(pluginName, nullptr);
        if (!pluginConfig) {
            continue;
        }

        if (!pluginObj.contains("configItems") || !pluginObj["configItems"].isArray()) {
            // todo - set defaults for this plugin config
            qWarning() << "ConfigItems array not found or is not an array in plugin:" << pluginName;
            continue;
        }

        const QJsonArray configItemsArray = pluginObj["configItems"].toArray();
        for (auto &p : pluginConfig->configItems) {
            auto savedConfig = findKey(configItemsArray, p.key);
            auto val = savedConfig.value("value");

            switch (p.type) {
            case qmdiConfigItem::StringList: {
                QJsonArray jsonArray = val.toArray();
                QStringList stringList;
                for (const auto &item : std::as_const(jsonArray)) {
                    if (item.isString()) {
                        stringList.append(item.toString());
                    }
                }
                p.value = stringList;
                break;
            }
            case qmdiConfigItem::Bool:
                p.value = val.toVariant().toBool();
                break;

            case qmdiConfigItem::Int8:
            case qmdiConfigItem::Int16:
            case qmdiConfigItem::Int32:
                p.value = val.toInteger();
                break;

            case qmdiConfigItem::UInt8:
            case qmdiConfigItem::UInt16:
            case qmdiConfigItem::UInt32:
                p.value = val.toVariant().toUInt();
                break;

            case qmdiConfigItem::Float:
            case qmdiConfigItem::Double:
                p.value = val.toDouble();
                break;

            case qmdiConfigItem::String:
                p.value = val.toString();
                break;
            }
        }
    }
}

qmdiPluginConfig *qmdiGlobalConfig::getPluginConfig(const QString &pluginName) const {
    return pluginMap.contains(pluginName) ? pluginMap[pluginName] : nullptr;
}

void qmdiGlobalConfig::addPluginConfig(qmdiPluginConfig *pluginConfig) {
    if (!pluginConfig || pluginMap.contains(pluginConfig->pluginName)) {
        qWarning() << "Invalid plugin configuration or plugin already exists.";
        return;
    }

    pluginMap[pluginConfig->pluginName] = pluginConfig;
    plugins.append(pluginConfig);
}
