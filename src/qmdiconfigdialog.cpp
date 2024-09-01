#include "qmdiconfigdialog.h"
#include "qmdiglobalconfig.h"
#include "qmdipluginconfig.h"

#include <QCheckBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStringListModel>
#include <QVBoxLayout>

qmdiConfigDialog::qmdiConfigDialog(qmdiGlobalConfig *config, QWidget *parent)
    : QDialog(parent), configLayout(new QVBoxLayout), mainLayout(new QHBoxLayout(this)),
      pluginListView(new QListView(this)), pluginModel(new QStringListModel(this)),
      buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this)),
      globalConfig(config) {
    setLayout(mainLayout);

    pluginListView->setModel(pluginModel);
    mainLayout->addWidget(pluginListView, 1);

    QWidget *configContainer = new QWidget(this);
    QVBoxLayout *configAndButtonsLayout = new QVBoxLayout(configContainer);

    configContainer->setLayout(configAndButtonsLayout);
    configAndButtonsLayout->addLayout(configLayout);

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    configAndButtonsLayout->addItem(spacer);
    configAndButtonsLayout->addWidget(buttonBox);
    mainLayout->addWidget(configContainer, 3);

    QStringList pluginNames;
    for (qmdiPluginConfig *pluginConfig : globalConfig->plugins) {
        pluginNames.append(pluginConfig->pluginName);
    }
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
    const qmdiPluginConfig *pluginConfig = globalConfig->getPluginConfig(pluginName);
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

    for (const qmdiConfigItem &item : pluginConfig->configItems) {
        QLabel *label = new QLabel(item.displayName, this);
        configLayout->addWidget(label);

        QWidget *widget = nullptr;
        switch (item.type) {
        case qmdiConfigItem::String:
            widget = new QLineEdit(this);
            static_cast<QLineEdit *>(widget)->setText(item.value.toString());
            break;
        case qmdiConfigItem::Bool:
            widget = new QCheckBox(this);
            static_cast<QCheckBox *>(widget)->setChecked(item.value.toBool());
            break;
        case qmdiConfigItem::Int8:
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(INT8_MIN);
            static_cast<QSpinBox *>(widget)->setMaximum(INT8_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::Int16:
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(INT16_MIN);
            static_cast<QSpinBox *>(widget)->setMaximum(INT16_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::Int32:
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(INT32_MIN);
            static_cast<QSpinBox *>(widget)->setMaximum(INT32_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::UInt8:
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(0);
            static_cast<QSpinBox *>(widget)->setMaximum(UINT8_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::UInt16:
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(0);
            static_cast<QSpinBox *>(widget)->setMaximum(UINT16_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::UInt32:
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(0);
            static_cast<QSpinBox *>(widget)->setMaximum(UINT32_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::Float:
            widget = new QDoubleSpinBox(this);
            static_cast<QDoubleSpinBox *>(widget)->setValue(item.value.toFloat());
            break;
        case qmdiConfigItem::Double:
            widget = new QDoubleSpinBox(this);
            static_cast<QDoubleSpinBox *>(widget)->setValue(item.value.toDouble());
            break;
        }

        if (widget) {
            configLayout->addWidget(widget);
            widgetMap[item.key] = widget;
        }
    }
}

void qmdiConfigDialog::onPluginSelectionChanged(const QModelIndex &index) {
    QString selectedPlugin = pluginModel->data(index, Qt::DisplayRole).toString();
    updateWidgetsForPlugin(selectedPlugin);
}

void qmdiConfigDialog::cancelConfiguration() { reject(); }

void qmdiConfigDialog::acceptChanges() {
    QString selectedPlugin =
        pluginModel->data(pluginListView->selectionModel()->currentIndex(), Qt::DisplayRole)
            .toString();

    qmdiPluginConfig *pluginConfig = globalConfig->getPluginConfig(selectedPlugin);
    if (!pluginConfig) {
        qWarning() << "No plugin config found for:" << selectedPlugin;
        reject();
        return;
    }

    for (qmdiConfigItem &configItem : pluginConfig->configItems) {
        if (widgetMap.contains(configItem.key)) {
            QWidget *widget = widgetMap[configItem.key];
            if (configItem.type == qmdiConfigItem::String) {
                configItem.value = static_cast<QLineEdit *>(widget)->text();
            } else if (configItem.type == qmdiConfigItem::Bool) {
                configItem.value = static_cast<QCheckBox *>(widget)->isChecked();
            } else if (configItem.type == qmdiConfigItem::UInt16) {
                configItem.value = static_cast<QSpinBox *>(widget)->value();
            } else if (configItem.type == qmdiConfigItem::Float) {
                configItem.value = static_cast<QDoubleSpinBox *>(widget)->value();
            }
        }
    }

    accept();
}
