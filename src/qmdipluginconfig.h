#pragma once

#include <QList>
#include <QString>
#include <QVariant>

class qmdiConfigItem {
  public:
    QString key;
    QString type;
    QString displayName;
    QString description;
    QVariant defaultValue;
    QVariant value;
};

class qmdiPluginConfig {
  public:
    QString pluginName;
    QString description;
    QList<qmdiConfigItem> configItems;

    qmdiPluginConfig() = default;

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
};
