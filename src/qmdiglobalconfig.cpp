#include "qmdiGlobalConfig.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

qmdiGlobalConfig::qmdiGlobalConfig(QObject *parent) : QObject(parent) {}

bool qmdiGlobalConfig::loadFromFile2(const QString &filePath) {
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
    QJsonArray pluginsArray = jsonObject["plugins"].toArray();

    // Clear existing plugins
    qDeleteAll(plugins);
    plugins.clear();
    pluginMap.clear();

    for (const QJsonValue &value : pluginsArray) {
        QJsonObject pluginObject = value.toObject();
        qmdiPluginConfig *pluginConfig = new qmdiPluginConfig();

        pluginConfig->pluginName = pluginObject["pluginName"].toString();
        pluginConfig->description = pluginObject["description"].toString();

        QJsonArray configItemsArray = pluginObject["configItems"].toArray();
        pluginConfig->configItems.clear();

        for (const QJsonValue &itemValue : configItemsArray) {
            QJsonObject itemObject = itemValue.toObject();
            qmdiConfigItem item;

            item.key = itemObject["key"].toString();
            item.type = itemObject["type"].toString();
            item.displayName = itemObject["displayName"].toString();
            item.description = itemObject["description"].toString();

            QJsonValue defaultValue = itemObject["defaultValue"];
            if (defaultValue.isString()) {
                item.defaultValue = defaultValue.toString();
            } else if (defaultValue.isBool()) {
                item.defaultValue = defaultValue.toBool();
            } else if (defaultValue.isDouble()) {
                item.defaultValue = defaultValue.toDouble();
            } else if (defaultValue.isDouble()) {
                item.defaultValue = defaultValue.toInt();
            } else {
                qWarning() << "Unsupported default value type.";
                delete pluginConfig;
                return false;
            }

            pluginConfig->configItems.append(item);
        }

        pluginMap[pluginConfig->pluginName] = pluginConfig;
        plugins.append(pluginConfig); // Append pointer to QList<qmdiPluginConfig*>
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

    // Iterate over each plugin in the JSON object
    for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it) {
        const QString pluginName = it.key();
        const QJsonObject pluginObj = it.value().toObject();

        qDebug() << "Processing plugin:" << pluginName;
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
        auto findKey = [configItemsArray](auto key) -> QJsonObject {
            for (const QJsonValue &itemValue : configItemsArray) {
                if (!itemValue.isObject()) {
                    continue;
                }
                const QJsonObject configItemObj = itemValue.toObject();
                if (configItemObj.contains("key") && configItemObj["key"].isString()) {
                    const QString k = configItemObj["key"].toString();
                    // const QString v = configItemObj["values"].toString();
                    // qDebug() << "Config item key:" << key;
                    if (k == key) {
                        return configItemObj;
                    }
                }
            }
            return {};
        };

        for (auto &p : pluginConfig->configItems) {
            auto savedConfig = findKey(p.key);
            if (savedConfig.empty()) {
                p.value = p.defaultValue;
            } else {
                // todo - port to the correct type
                p.value = savedConfig.value("value").toString();
            }
        }
    }
    return true;
}

bool qmdiGlobalConfig::saveToFile(const QString &filePath) {
    QJsonObject jsonObject;

    // Iterate over all plugins and convert their configuration to JSON
    for (auto it = pluginMap.begin(); it != pluginMap.end(); ++it) {
        const qmdiPluginConfig *pluginConfig = it.value();
        QJsonObject pluginObject;
        pluginObject["description"] = pluginConfig->description;

        QJsonArray configItemsArray;
        for (const qmdiConfigItem &item : pluginConfig->configItems) {
            QJsonObject itemObject;
            itemObject["key"] = item.key;
            itemObject["type"] = item.type;
            itemObject["displayName"] = item.displayName;
            itemObject["value"] = item.value.toString();
            // itemObject["description"] = item.description;
            // itemObject["defaultValue"] = QJsonValue::fromVariant(item.defaultValue);
            configItemsArray.append(itemObject);
        }

        pluginObject["configItems"] = configItemsArray;
        jsonObject[pluginConfig->pluginName] = pluginObject;
    }

    // Convert QJsonObject to QJsonDocument
    QJsonDocument jsonDoc(jsonObject);

    // Write JSON document to file
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error opening file for writing:" << file.errorString();
        return false;
    }

    file.write(jsonDoc.toJson());
    file.close();
    return true;
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
