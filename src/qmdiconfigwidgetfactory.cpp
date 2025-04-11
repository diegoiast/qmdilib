/**
 * \file qmdiconfigwidgetfactory.cpp
 * \brief Widget factory implementation
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 */

#include "qmdiconfigwidgetfactory.h"
#include "pathwidget.h"
#include "qmdidialogevents.hpp"

QWidget *qmdiDefaultConfigWidgetFactory::createWidget(const qmdiConfigItem &item, QWidget *parent) {
    QWidget *widget = nullptr;

    switch (item.type) {
    case qmdiConfigItem::String: {
        auto *lineEdit = new QLineEdit(parent);
        lineEdit->setText(item.value.toString());
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
    case qmdiConfigItem::OneOf: {
        auto *comboBox = new QComboBox(parent);
        comboBox->addItems(item.possibleValue.toStringList());
        comboBox->setCurrentIndex(item.value.toInt());
        widget = comboBox;
    } break;
    case qmdiConfigItem::Font: {
        auto *label = new QLabel(parent);
        auto font = label->font();
        font.fromString(item.value.toString());
        label->setFont(font);
        label->setProperty("isFontWidget", true);
        label->setFrameShape(QFrame::StyledPanel);
        label->setText(item.value.toString());
        widget = label;
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
        QObject::connect(button, &QPushButton::clicked, button,
                         [item]() { qmdiDialogEvents::instance().buttonClicked(item.key); });
        widget = button;
    } break;
    case qmdiConfigItem::Label: {
        auto *label = new QLabel(parent);
        label->setText(item.value.toString());
        QObject::connect(label, &QLabel::linkActivated, label, [item](const QString &link) {
            qmdiDialogEvents::instance().linkClicked(item.key, link);
        });
        widget = label;
    }
    case qmdiConfigItem::Last:
        break;
    }

    if (widget && !item.description.isEmpty()) {
        widget->setToolTip(item.description);
    }

    return widget;
}

QLabel *qmdiDefaultConfigWidgetFactory::createLabel(const qmdiConfigItem &item, QWidget *parent) {
    if (item.type == qmdiConfigItem::Bool) {
        // Checkbox includes its own label
        return nullptr;
    }
    return new QLabel(item.displayName, parent);
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
    if (auto *label = qobject_cast<QLabel *>(widget)) {
        if (label->property("isFontWidget").toBool()) {
            return label->font().toString();
        }
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
    } else if (auto *label = qobject_cast<QLabel *>(widget)) {
        if (label->property("isFontWidget").toBool()) {
            auto font = label->font();
            font.fromString(value.toString());
            label->setFont(font);
            label->setText(value.toString());
        }
    } else if (auto *pathWidget = qobject_cast<PathWidget *>(widget)) {
        pathWidget->setPath(value.toString());
    }
}

qmdiConfigWidgetRegistry &qmdiConfigWidgetRegistry::instance() {
    static qmdiConfigWidgetRegistry registry;
    return registry;
}

void qmdiConfigWidgetRegistry::registerFactory(qmdiConfigItem::ClassType type,
                                               FactoryCreator creator) {
    factories[type] = creator;
}

std::unique_ptr<qmdiConfigWidgetFactory>
qmdiConfigWidgetRegistry::createFactory(qmdiConfigItem::ClassType type) {
    ensureDefaultFactoriesRegistered();
    auto it = factories.find(type);
    if (it != factories.end()) {
        return it->second();
    }
    return nullptr;
}

qmdiConfigWidgetRegistry::qmdiConfigWidgetRegistry() { ensureDefaultFactoriesRegistered(); }

void qmdiConfigWidgetRegistry::ensureDefaultFactoriesRegistered() {
    if (!defaultFactoriesRegistered) {
        auto registerDefault = [this](qmdiConfigItem::ClassType type) {
            registerFactory(type,
                            []() { return std::make_unique<qmdiDefaultConfigWidgetFactory>(); });
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
        registerDefault(qmdiConfigItem::OneOf);
        registerDefault(qmdiConfigItem::Font);
        registerDefault(qmdiConfigItem::Path);
        registerDefault(qmdiConfigItem::Button);
        registerDefault(qmdiConfigItem::Label);

        defaultFactoriesRegistered = true;
    }
}
