#include "qmdiConfigDialog.h"
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
    // Clear existing widgets in layout
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
        if (item.type == "string") {
            widget = new QLineEdit(this);
            static_cast<QLineEdit *>(widget)->setText(item.value.toString());
        } else if (item.type == "boolean") {
            widget = new QCheckBox(this);
            static_cast<QCheckBox *>(widget)->setChecked(item.value.toBool());
        } else if (item.type == "number") {
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMaximum(0xffff);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
        } else if (item.type == "float") {
            widget = new QDoubleSpinBox(this);
            static_cast<QDoubleSpinBox *>(widget)->setValue(item.value.toDouble());
        }

        if (widget) {
            configLayout->addWidget(widget);
            widgetMap[item.key] = widget; // Store the widget in the map
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
            if (configItem.type == "string") {
                configItem.value = static_cast<QLineEdit *>(widget)->text();
            } else if (configItem.type == "boolean") {
                configItem.value = static_cast<QCheckBox *>(widget)->isChecked();
            } else if (configItem.type == "number") {
                configItem.value = static_cast<QSpinBox *>(widget)->value();
            } else if (configItem.type == "float") {
                configItem.value = static_cast<QDoubleSpinBox *>(widget)->value();
            }
        }
    }

    accept();
}
