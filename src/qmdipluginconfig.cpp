#include "qmdiPluginConfig.h"

qmdiConfigItem::qmdiConfigItem() {}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setKey(const QString &key) {
    this->key = key;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setType(const QString &type) {
    this->type = type;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setDisplayName(const QString &displayName) {
    this->displayName = displayName;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setDescription(const QString &description) {
    this->description = description;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setDefaultValue(const QVariant &defaultValue) {
    this->defaultValue = defaultValue;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setValue(const QVariant &value) {
    this->value = value;
    return *this;
}

qmdiConfigItem qmdiConfigItem::Builder::build() const {
    qmdiConfigItem item;
    item.key = key;
    item.type = type;
    item.displayName = displayName;
    item.description = description;
    item.defaultValue = defaultValue;
    item.value = value.isNull() ? defaultValue : value;
    return item;
}
