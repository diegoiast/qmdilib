/**
 * \file qmdiconfigwidgetfactory.cpp
 * \brief Widget factory implementation
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 */

#include <QJsonArray>
#include <QMouseEvent>

#include "fontwidget.hpp"
#include "pathwidget.h"
#include "stringlistwidget.h"
#include "qmdiconfigwidgetfactory.h"
#include "qmdidialogevents.hpp"

QLabel *qmdiDefaultConfigWidgetFactory::createLabel(const qmdiConfigItem &item,
                                                    qmdiConfigDialog *parent) {
    // Checkbox includes its own label
    if (item.type == qmdiConfigItem::Bool) {
        return nullptr;
    }
    // Buttons do not need label
    if (item.type == qmdiConfigItem::Button) {
        return nullptr;
    }

    auto label = new QLabel(item.displayName, parent);
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    parent->connect(label, &QLabel::linkActivated, [parent, item](const QString &link) {
        qmdiDialogEvents::instance().linkClicked(parent, item.key, link);
    });
    return label;
}

QWidget *qmdiDefaultConfigWidgetFactory::createWidget(const qmdiConfigItem &item,
                                                      qmdiConfigDialog *parent) {
    QWidget *widget = nullptr;

    switch (item.type) {
    case qmdiConfigItem::String: {
        auto *lineEdit = new QLineEdit(parent);
        lineEdit->setText(item.value.toString());
        lineEdit->setPlaceholderText(item.description);
        widget = lineEdit;
    } break;
    case qmdiConfigItem::Bool: {
        auto *checkBox = new QCheckBox(parent);
        checkBox->setChecked(item.value.toBool());
        checkBox->setText(item.displayName);
        widget = checkBox;
    } break;
    case qmdiConfigItem::Int8:
    case qmdiConfigItem::Int16:
    case qmdiConfigItem::Int32: {
        auto *spinBox = new QSpinBox(parent);
        if (item.type == qmdiConfigItem::Int8) {
            spinBox->setRange(INT8_MIN, INT8_MAX);
        } else if (item.type == qmdiConfigItem::Int16) {
            spinBox->setRange(INT16_MIN, INT16_MAX);
        } else {
            spinBox->setRange(INT32_MIN, INT32_MAX);
        }
        spinBox->setValue(item.value.toInt());
        widget = spinBox;
    } break;
    case qmdiConfigItem::UInt8:
    case qmdiConfigItem::UInt16:
    case qmdiConfigItem::UInt32: {
        auto *spinBox = new QSpinBox(parent);
        spinBox->setMinimum(0);
        if (item.type == qmdiConfigItem::UInt8) {
            spinBox->setMaximum(UINT8_MAX);
        } else if (item.type == qmdiConfigItem::UInt16) {
            spinBox->setMaximum(UINT16_MAX);
        } else {
            spinBox->setMaximum(INT32_MAX); // Qt limitation
        }
        spinBox->setValue(item.value.toInt());
        widget = spinBox;
    } break;
    case qmdiConfigItem::Float:
    case qmdiConfigItem::Double: {
        auto *doubleSpinBox = new QDoubleSpinBox(parent);
        if (item.type == qmdiConfigItem::Float) {
            doubleSpinBox->setValue(item.value.toFloat());
        } else {
            doubleSpinBox->setValue(item.value.toDouble());
        }
        widget = doubleSpinBox;
    } break;
    case qmdiConfigItem::StringList: {
        auto *stringListWidget = new StringListWidget(parent);
        stringListWidget->setList(item.value.toStringList());
        widget = stringListWidget;
    } break;
    case qmdiConfigItem::PathList: {
        auto *stringListWidget = new StringListWidget(parent);
        stringListWidget->setList(item.value.toStringList());
        stringListWidget->setMode(StringListMode::Directory);
        widget = stringListWidget;
    } break;
    case qmdiConfigItem::FileList: {
        auto *stringListWidget = new StringListWidget(parent);
        stringListWidget->setList(item.value.toStringList());
        stringListWidget->setMode(StringListMode::File);
        widget = stringListWidget;
    } break;
    case qmdiConfigItem::OneOf: {
        auto *comboBox = new QComboBox(parent);
        comboBox->addItems(item.possibleValue.toStringList());
        comboBox->setCurrentIndex(item.value.toInt());
        widget = comboBox;
    } break;
    case qmdiConfigItem::Font: {
        auto str = item.value.toString();
        widget = new FontWidget(parent, str);
    } break;
    case qmdiConfigItem::Path: {
        auto *pathWidget = new PathWidget(parent);
        pathWidget->setPath(item.value.toString());
        if (!item.possibleValue.isNull()) {
            bool isFile = item.possibleValue.toBool();
            pathWidget->setFileMode(isFile);
            pathWidget->setValidateExistence(true);
        }
        widget = pathWidget;
    } break;
    case qmdiConfigItem::Button: {
        auto *button = new QPushButton(item.displayName, parent);
        if (!item.defaultValue.toString().isEmpty()) {
            button->setIcon(QIcon::fromTheme(item.defaultValue.toString()));
        }
        QObject::connect(button, &QPushButton::clicked, button, [parent, item]() {
            qmdiDialogEvents::instance().buttonClicked(parent, item.key);
        });
        widget = button;
    } break;
    case qmdiConfigItem::Json:
    case qmdiConfigItem::Custom: {
        // JSON and custom - are special, we assume they are store for random
        // data. We do not provide edit widget for them.
        break;
    }
    case qmdiConfigItem::Label: {
        // labels are special case - no actual edit widget is created
        break;
    }
    case qmdiConfigItem::Last:
        break;
    }

    if (widget && !item.description.isEmpty()) {
        widget->setToolTip(item.description);
    }

    return widget;
}

QVariant qmdiDefaultConfigWidgetFactory::getValue(QWidget *widget) {
    if (auto *lineEdit = qobject_cast<QLineEdit *>(widget)) {
        return lineEdit->text();
    }
    if (auto *checkBox = qobject_cast<QCheckBox *>(widget)) {
        return checkBox->isChecked();
    }
    if (auto *spinBox = qobject_cast<QSpinBox *>(widget)) {
        return spinBox->value();
    }
    if (auto *doubleSpinBox = qobject_cast<QDoubleSpinBox *>(widget)) {
        return doubleSpinBox->value();
    }
    if (auto *stringListWidget = qobject_cast<StringListWidget *>(widget)) {
        return stringListWidget->getList();
    }
    if (auto *comboBox = qobject_cast<QComboBox *>(widget)) {
        return comboBox->currentIndex();
    }
    if (auto *label = qobject_cast<FontWidget *>(widget)) {
        return label->font().toString();
    }
    if (auto *pathWidget = qobject_cast<PathWidget *>(widget)) {
        return pathWidget->path();
    }
    if (auto *button = qobject_cast<QPushButton *>(widget)) {
        return button->text();
    }
    if (auto *label = qobject_cast<QLabel *>(widget)) {
        return label->text();
    }
    return {};
}

void qmdiDefaultConfigWidgetFactory::setValue(QWidget *widget, const QVariant &value) {
    if (auto *lineEdit = qobject_cast<QLineEdit *>(widget)) {
        lineEdit->setText(value.toString());
    } else if (auto *checkBox = qobject_cast<QCheckBox *>(widget)) {
        checkBox->setChecked(value.toBool());
    } else if (auto *spinBox = qobject_cast<QSpinBox *>(widget)) {
        spinBox->setValue(value.toInt());
    } else if (auto *doubleSpinBox = qobject_cast<QDoubleSpinBox *>(widget)) {
        doubleSpinBox->setValue(value.toDouble());
    } else if (auto *button = qobject_cast<QPushButton *>(widget)) {
        button->setText(value.toString());
    } else if (auto *stringListWidget = qobject_cast<StringListWidget *>(widget)) {
        stringListWidget->setList(value.toStringList());
    } else if (auto *comboBox = qobject_cast<QComboBox *>(widget)) {
        comboBox->setCurrentIndex(value.toInt());
    } else if (auto *label = qobject_cast<FontWidget *>(widget)) {
        auto font = label->font();
        font.fromString(value.toString());
        label->setFont(font);
        label->setText(value.toString());
    } else if (auto *button = qobject_cast<QPushButton *>(widget)) {
        button->setText(value.toString());
    } else if (auto *pathWidget = qobject_cast<PathWidget *>(widget)) {
        pathWidget->setPath(value.toString());
    }
}

QVariant qmdiDefaultConfigWidgetFactory::parse(const qmdiConfigItem &item, const QJsonValue &v) {
    switch (item.type) {
    case qmdiConfigItem::String:
        return v.toString();
    case qmdiConfigItem::Bool:
        if (v.isBool()) return v.toBool();
        return v.toString().toLower() == "true";
    case qmdiConfigItem::Int8:
    case qmdiConfigItem::Int16:
    case qmdiConfigItem::Int32:
        if (v.isDouble()) return v.toInt();
        return v.toString().toInt();
    case qmdiConfigItem::UInt8:
    case qmdiConfigItem::UInt16:
    case qmdiConfigItem::UInt32:
        if (v.isDouble()) return v.toInt();
        return v.toString().toInt();
    case qmdiConfigItem::Float:
    case qmdiConfigItem::Double:
        if (v.isDouble()) return v.toDouble();
        return v.toString().toDouble();
        return v.toDouble();
    case qmdiConfigItem::StringList:
    case qmdiConfigItem::PathList:
    case qmdiConfigItem::FileList:
        return v.toVariant().toStringList();
    case qmdiConfigItem::OneOf:
        return v.toVariant();
    case qmdiConfigItem::Font:
        return v.toString();
    case qmdiConfigItem::Path:
        return v.toString();
    case qmdiConfigItem::Json:
        return v.toVariant();
    case qmdiConfigItem::Custom:
        return v.toVariant();
    case qmdiConfigItem::Button:
    case qmdiConfigItem::Label:
    case qmdiConfigItem::Last:
        break;
    }
    return {};
}

QJsonValue qmdiDefaultConfigWidgetFactory::serialize(const qmdiConfigItem &item,
                                                     const QVariant &v) {
    if (item.type == qmdiConfigItem::Json) {
        return QJsonValue::fromVariant(v);
    }
    if (item.type == qmdiConfigItem::StringList || item.type == qmdiConfigItem::PathList || item.type == qmdiConfigItem::FileList) {
        QStringList stringList = v.toStringList();
        QJsonArray jsonArray;
        for (const QString &str : std::as_const(stringList)) {
            jsonArray.append(str);
        }
        return jsonArray;
    }
    if (item.type == qmdiConfigItem::Bool) {
        return v.toBool();
    }
    return v.toString();
}

qmdiConfigWidgetRegistry &qmdiConfigWidgetRegistry::instance() {
    static qmdiConfigWidgetRegistry registry;
    return registry;
}

void qmdiConfigWidgetRegistry::registerFactory(qmdiConfigItem::ClassType type,
                                               FactoryCreator creator) {
    factories[type] = creator;
}

void qmdiConfigWidgetRegistry::registerCustomFactory(const QString &customType,
                                                     FactoryCreator creator) {
    customFactories[customType] = creator;
}

void qmdiConfigWidgetRegistry::clearCustomFactories() { customFactories.clear(); }

std::unique_ptr<qmdiConfigWidgetFactory>
qmdiConfigWidgetRegistry::getHandler(const qmdiConfigItem &item) {
    ensureDefaultFactoriesRegistered();

    auto createSafe =
        [](const FactoryCreator &creator) -> std::unique_ptr<qmdiConfigWidgetFactory> {
        if (creator) {
            if (auto handler = creator()) {
                return handler;
            }
        }
        return std::make_unique<qmdiDefaultConfigWidgetFactory>();
    };

    if (item.type == qmdiConfigItem::Custom && !item.customTypeString.isEmpty()) {
        auto it = customFactories.find(item.customTypeString);
        if (it != customFactories.end()) {
            return createSafe(it.value());
        }
    }

    auto it = factories.find(item.type);
    if (it != factories.end()) {
        return createSafe(it->second);
    }

    return std::make_unique<qmdiDefaultConfigWidgetFactory>();
}

QWidget *qmdiConfigWidgetRegistry::createWidget(const qmdiConfigItem &item,
                                                qmdiConfigDialog *parent) {
    auto handler = getHandler(item);
    return handler ? handler->createWidget(item, parent) : nullptr;
}

QLabel *qmdiConfigWidgetRegistry::createLabel(const qmdiConfigItem &item,
                                              qmdiConfigDialog *parent) {
    auto handler = getHandler(item);
    return handler ? handler->createLabel(item, parent) : nullptr;
}

QVariant qmdiConfigWidgetRegistry::getValue(const qmdiConfigItem &item, QWidget *widget) {
    auto handler = getHandler(item);
    return (handler && widget) ? handler->getValue(widget) : QVariant();
}

void qmdiConfigWidgetRegistry::setValue(const qmdiConfigItem &item, QWidget *widget,
                                        const QVariant &value) {
    auto handler = getHandler(item);
    if (handler && widget) {
        handler->setValue(widget, value);
    }
}

QVariant qmdiConfigWidgetRegistry::parse(const qmdiConfigItem &item, const QJsonValue &v) {
    auto handler = getHandler(item);
    return handler ? handler->parse(item, v) : QVariant();
}

QJsonValue qmdiConfigWidgetRegistry::serialize(const qmdiConfigItem &item, const QVariant &v) {
    auto handler = getHandler(item);
    return handler ? handler->serialize(item, v) : QJsonValue();
}

qmdiConfigWidgetRegistry::qmdiConfigWidgetRegistry() { ensureDefaultFactoriesRegistered(); }

void qmdiConfigWidgetRegistry::ensureDefaultFactoriesRegistered() {
    if (defaultFactoriesRegistered) {
        return;
    }
    auto registerDefault = [this](qmdiConfigItem::ClassType type) {
        registerFactory(type, []() { return std::make_unique<qmdiDefaultConfigWidgetFactory>(); });
    };
    registerDefault(qmdiConfigItem::String);
    registerDefault(qmdiConfigItem::Bool);
    registerDefault(qmdiConfigItem::Int8);
    registerDefault(qmdiConfigItem::Int16);
    registerDefault(qmdiConfigItem::Int32);
    registerDefault(qmdiConfigItem::UInt8);
    registerDefault(qmdiConfigItem::UInt16);
    registerDefault(qmdiConfigItem::UInt32);
    registerDefault(qmdiConfigItem::Float);
    registerDefault(qmdiConfigItem::Double);
    registerDefault(qmdiConfigItem::StringList);
    registerDefault(qmdiConfigItem::PathList);
    registerDefault(qmdiConfigItem::OneOf);
    registerDefault(qmdiConfigItem::Font);
    registerDefault(qmdiConfigItem::Path);
    registerDefault(qmdiConfigItem::Button);
    registerDefault(qmdiConfigItem::Label);
    defaultFactoriesRegistered = true;
}
