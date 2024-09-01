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
        Double
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

    // Nested builder class
    struct Builder {
        Builder &setKey(const QString &key);
        Builder &setType(const ClassType type);
        Builder &setDisplayName(const QString &displayName);
        Builder &setDescription(const QString &description);
        Builder &setDefaultValue(const QVariant &defaultValue);
        Builder &setValue(const QVariant &value);

        qmdiConfigItem build() const;

      private:
        QString key;
        ClassType type;
        QString displayName;
        QString description;
        QVariant defaultValue;
        QVariant value;
    };
    void setDefault();
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
