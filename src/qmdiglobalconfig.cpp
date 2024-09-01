#include "qmdiglobalconfig.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

qmdiGlobalConfig::qmdiGlobalConfig(QObject *parent) : QObject(parent) {}

void qmdiGlobalConfig::setDefaults() {
    for (auto it = pluginMap.begin(); it != pluginMap.end(); ++it) {
        const qmdiPluginConfig *pluginConfig = it.value();

        for (auto config : pluginConfig->configItems) {
            config.setDefault();
        }
    }
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
            itemObject["value"] = item.value.toString();
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
            if (savedConfig.empty()) {
                p.value = p.defaultValue;
            } else {
                // todo - port to the correct type
                p.value = savedConfig.value("value").toString();
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
