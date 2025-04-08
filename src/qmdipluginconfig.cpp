/**
 * \file qmdipluginconfig.h
 * \brief Implementation of the plugin config
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#include "qmdipluginconfig.h"

qmdiConfigItem::qmdiConfigItem() : type(String), userEditable(true) {}

void qmdiConfigItem::setDefault() { this->value = this->defaultValue; }

qmdiConfigItem::ClassType qmdiConfigItem::typeFromString(const QString &typeStr) {
    auto lower = typeStr.toLower();
    if (lower == "string") {
        return String;
    } else if (lower == "bool" || lower == "boolean") {
        return Bool;
    } else if (lower == "int8") {
        return Int8;
    } else if (lower == "int16") {
        return Int16;
    } else if (lower == "int32") {
        return Int32;
    } else if (lower == "uint8") {
        return UInt8;
    } else if (lower == "uint16" || lower == "number") {
        return UInt16;
    } else if (lower == "uint32") {
        return UInt32;
    } else if (lower == "float") {
        return Float;
    } else if (lower == "double") {
        return Double;
    } else if (lower == "stringlist") {
        return StringList;
    } else if (lower == "oneof") {
        return OneOf;
    } else if (lower == "font") {
        return Font;
    } else if (lower == "path") {
        return Path;
    } else if (lower == "last") {
        return Last;
    } else {
        return String;
    }
}

QString qmdiConfigItem::typeToString(ClassType type) {
    switch (type) {
    case String:
        return "String";
    case Bool:
        return "Bool";
    case Int8:
        return "Int8";
    case Int16:
        return "Int16";
    case Int32:
        return "Int32";
    case UInt8:
        return "UInt8";
    case UInt16:
        return "UInt16";
    case UInt32:
        return "UInt32";
    case Float:
        return "Float";
    case Double:
        return "Double";
    case StringList:
        return "StringList";
    case OneOf:
        return "OneOf";
        break;
    case Font:
        return "Font";
        break;
    case Path:
        return "Path";
        break;
    case Last:
        return "Last";
        break;
    }
    return "Unknown";
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setKey(const QString &key) {
    this->key = key;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setType(const ClassType type) {
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

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setDefaultValue(const QVariant defaultValue) {
    this->defaultValue = defaultValue;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setUserEditable(const bool value) {
    this->userEditable = value;
    return *this;
}

qmdiConfigItem::Builder &qmdiConfigItem::Builder::setPossibleValue(const QVariant &value) {
    this->possibleValue = value;
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
    item.userEditable = userEditable;
    item.possibleValue = possibleValue;
    return item;
}

void qmdiPluginConfig::setDefault() {
    for (auto &item : configItems) {
        item.setDefault();
    }
}

int qmdiPluginConfig::editableConfigs() const {
    auto editable = 0;
    for (auto &item : configItems) {
        if (item.userEditable) {
            editable++;
        }
    }
    return editable;
}
