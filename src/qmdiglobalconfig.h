#pragma once

#include "qmdipluginconfig.h"
#include <QJsonObject>
#include <QMap>
#include <QObject>

class qmdiGlobalConfig : public QObject {

  public:
    explicit qmdiGlobalConfig(QObject *parent = nullptr);
    void setDefaults();
    bool loadDefsFromFile(const QString &filePath);
    bool loadDefsFromJson(const QJsonObject &jsonObject);
    bool loadFromFile(const QString &filePath);
    bool loadFromJson(const QJsonObject &jsonObject) {
        fromJson(jsonObject);
        return true;
    }
    bool saveToFile(const QString &filePath);

    QJsonObject asJson() const;
    void fromJson(QJsonObject o);

    qmdiPluginConfig *getPluginConfig(const QString &pluginName) const;
    void addPluginConfig(qmdiPluginConfig *pluginConfig);

    template <typename T> T getVariable(const QString &pluginName, const QString &key) const {
        const qmdiPluginConfig *pluginConfig = getPluginConfig(pluginName);
        if (pluginConfig) {
            return pluginConfig->getVariable<T>(key);
        }
        throw std::runtime_error("Plugin not found or key not found");
    }

    template <typename T> void setVariable(const QString &pluginName, const QString &key, T value) {
        qmdiPluginConfig *pluginConfig = getPluginConfig(pluginName);
        if (pluginConfig) {
            return pluginConfig->setVariable<T>(key, value);
        }
        throw std::runtime_error("Plugin not found or key not found");
    }

    QList<qmdiPluginConfig *> plugins;

  private:
    QMap<QString, qmdiPluginConfig *> pluginMap;
};
