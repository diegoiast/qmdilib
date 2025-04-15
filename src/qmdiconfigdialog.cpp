/**
 * \file qmdiconfigdialog.cpp
 * \brief Implementation config dialog
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#include "qmdiconfigdialog.h"
#include "qmdiconfigwidgetfactory.h"
#include "qmdidialogevents.hpp"
#include "qmdiglobalconfig.h"
#include "qmdipluginconfig.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QWidget>

qmdiConfigDialog::qmdiConfigDialog(qmdiGlobalConfig *config, QWidget *parent)
    : QDialog(parent), configLayout(new QVBoxLayout), mainLayout(new QHBoxLayout(this)),
      pluginListView(new QListView(this)), pluginModel(new QStringListModel(this)),
      buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this)),
      globalConfig(config) {
    setLayout(mainLayout);

    setWindowTitle(tr("Config"));
    pluginListView->setModel(pluginModel);
    mainLayout->addWidget(pluginListView, 1);

    auto configContainer = new QWidget(this);
    auto configAndButtonsLayout = new QVBoxLayout(configContainer);

    configContainer->setLayout(configAndButtonsLayout);
    configAndButtonsLayout->addLayout(configLayout);
    configAndButtonsLayout->setContentsMargins(0, 0, 0, 0);

    auto spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    configAndButtonsLayout->addItem(spacer);
    configAndButtonsLayout->addWidget(buttonBox);
    mainLayout->addWidget(configContainer, 3);

    QStringList pluginNames;
    for (auto const pluginConfig : std::as_const(globalConfig->plugins)) {
        if (pluginConfig->editableConfigs() == 0) {
            continue;
        }
        pluginNames.append(pluginConfig->pluginName);
    }
    pluginListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pluginModel->setStringList(pluginNames);

    if (!pluginNames.isEmpty()) {
        pluginListView->selectionModel()->setCurrentIndex(pluginModel->index(0),
                                                          QItemSelectionModel::SelectCurrent);
        updateWidgetsForPlugin(pluginNames.first());
    }

    connect(pluginListView->selectionModel(), &QItemSelectionModel::currentChanged, this,
            &qmdiConfigDialog::onPluginSelectionChanged);

    connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this,
            &qmdiConfigDialog::cancelConfiguration);
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this,
            &qmdiConfigDialog::acceptChanges);
}

qmdiConfigDialog::~qmdiConfigDialog() { qDeleteAll(widgetMap); }

void qmdiConfigDialog::updateWidgetsForPlugin(const QString &pluginName) {
    auto const pluginConfig = globalConfig->getPluginConfig(pluginName);
    if (pluginConfig) {
        createWidgetsFromConfig(pluginConfig);
    }
}

void qmdiConfigDialog::createWidgetsFromConfig(const qmdiPluginConfig *pluginConfig) {
    QLayoutItem *item;

    while ((item = configLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }
    widgetMap.clear();

    for (auto const item : pluginConfig->configItems) {
        if (!item.userEditable && !item.forceShow) {
            continue;
        }

        auto factory = qmdiConfigWidgetRegistry::instance().createFactory(item.type);
        if (!factory) {
            qWarning() << "No factory registered for type:" << item.type;
            continue;
        }

        auto widget = factory->createWidget(item, this);
        auto label = factory->createLabel(item, this);

        if (label) {
            configLayout->addWidget(label);
        }
        if (widget) {
            configLayout->addWidget(widget);
            widgetMap[item.key] = widget;
            widget->installEventFilter(this);
        }
    }
}

void qmdiConfigDialog::onPluginSelectionChanged(const QModelIndex &index) {
    QString selectedPlugin = pluginModel->data(index, Qt::DisplayRole).toString();
    updateWidgetsForPlugin(selectedPlugin);
}

void qmdiConfigDialog::cancelConfiguration() { reject(); }

void qmdiConfigDialog::acceptChanges() {
    auto selectedPlugin = pluginModel->data(pluginListView->currentIndex()).toString();
    auto pluginConfig = globalConfig->getPluginConfig(selectedPlugin);

    if (!pluginConfig) {
        qWarning() << "No plugin config found for:" << selectedPlugin;
        reject();
        return;
    }

    for (auto &item : pluginConfig->configItems) {
        if (!item.userEditable && !item.forceShow) {
            continue;
        }

        auto widget = widgetMap.value(item.key);
        if (!widget) {
            continue;
        }

        auto factory = qmdiConfigWidgetRegistry::instance().createFactory(item.type);
        if (!factory) {
            qWarning() << "No factory found for type:" << item.type;
            continue;
        }

        item.value = factory->getValue(widget);
    }

    accept();
}

bool qmdiConfigDialog::eventFilter(QObject *o, QEvent *e) {
    if (o->property("isFontWidget").toBool()) {
        auto w = qobject_cast<QWidget *>(o);
        if (e->type() == QEvent::MouseButtonRelease) {
            auto ok = false;
            auto newFont = QFontDialog::getFont(&ok, w->font(), this);
            if (ok) {
                w->setFont(newFont);
                static_cast<QLabel *>(w)->setText(newFont.toString());
            }
        }
    }
    return QDialog::eventFilter(o, e);
}

void qmdiConfigDialog::updateWidgetValues() {
    auto selectedPlugin = pluginModel->data(pluginListView->currentIndex()).toString();
    auto pluginConfig = globalConfig->getPluginConfig(selectedPlugin);
    if (!pluginConfig) {
        return;
    }

    for (auto &item : pluginConfig->configItems) {
        if (!item.userEditable && !item.forceShow) {
            continue;
        }

        auto widget = widgetMap.value(item.key);
        if (!widget) {
            continue;
        }

        auto factory = qmdiConfigWidgetRegistry::instance().createFactory(item.type);
        if (!factory) {
            continue;
        }

        factory->setValue(widget, item.value);
    }
}
