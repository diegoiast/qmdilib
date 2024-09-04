/**
 * \file qmdipluginconfig.h
 * \brief Implementation of the global config
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#pragma once

#include "qmdipluginconfig.h"
#include <QJsonObject>
#include <QMap>
#include <QObject>

/**
 * @brief Global configuration for a program
 *
 * This class lets you define a configuration for your program. You can save/load the modifications
 * from the default configuration, and also if needed restore the configuration to the defaults.
 *
 * See \file main3.cpp or unit test for examples of usages.
 */
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
        // TODO - this is out of line with the library behiaviour
        throw std::runtime_error("Plugin not found or key not found");
    }

    template <typename T> void setVariable(const QString &pluginName, const QString &key, T value) {
        qmdiPluginConfig *pluginConfig = getPluginConfig(pluginName);
        if (pluginConfig) {
            return pluginConfig->setVariable<T>(key, value);
        }
        // TODO - this is out of line with the library behiaviour
        throw std::runtime_error("Plugin not found or key not found");
    }

    QList<qmdiPluginConfig *> plugins;

  private:
    QMap<QString, qmdiPluginConfig *> pluginMap;
};
