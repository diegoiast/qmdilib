#pragma once

#include "qmdipluginconfig.h"
#include <QMap>
#include <QObject>

class qmdiGlobalConfig : public QObject {

  public:
    explicit qmdiGlobalConfig(QObject *parent = nullptr);
    void setDefaults();
    bool loadFromFile(const QString &filePath);
    bool saveToFile(const QString &filePath);

    qmdiPluginConfig *getPluginConfig(const QString &pluginName) const;
    void addPluginConfig(qmdiPluginConfig *pluginConfig);

    template <typename T> T getVariable(const QString &pluginName, const QString &key) const {
        const qmdiPluginConfig *pluginConfig = getPluginConfig(pluginName);
        if (pluginConfig) {
            return pluginConfig->getVariable<T>(key);
        }
        throw std::runtime_error("Plugin not found or key not found");
    }

    QList<qmdiPluginConfig *> plugins;

  private:
    QMap<QString, qmdiPluginConfig *> pluginMap;
};
