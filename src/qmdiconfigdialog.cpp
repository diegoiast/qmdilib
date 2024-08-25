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
    // Set up the main layout
    setLayout(mainLayout);

    // Set up the plugin list view
    pluginListView->setModel(pluginModel);
    mainLayout->addWidget(pluginListView, 1); // Stretch factor for the list view

    // Set up the configuration area
    QWidget *configContainer = new QWidget(this);
    QVBoxLayout *configAndButtonsLayout = new QVBoxLayout(configContainer);

    configContainer->setLayout(configAndButtonsLayout);
    configAndButtonsLayout->addLayout(configLayout); // Add the config layout

    // Add a spacer to push the buttons to the bottom
    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    configAndButtonsLayout->addItem(spacer);

    // Set up the button box
    configAndButtonsLayout->addWidget(buttonBox); // Add the button box below the config layout

    mainLayout->addWidget(configContainer, 3); // Stretch factor for the config area

    // Populate the list view with plugin names
    QStringList pluginNames;
    for (qmdiPluginConfig *pluginConfig : globalConfig->plugins) {
        pluginNames.append(pluginConfig->pluginName);
    }
    pluginModel->setStringList(pluginNames);

    // Automatically select the first plugin if the list is not empty
    if (!pluginNames.isEmpty()) {
        pluginListView->selectionModel()->setCurrentIndex(pluginModel->index(0),
                                                          QItemSelectionModel::SelectCurrent);
        updateWidgetsForPlugin(pluginNames.first());
    }

    // Connect the list view selection change signal
    connect(pluginListView->selectionModel(), &QItemSelectionModel::currentChanged, this,
            &qmdiConfigDialog::onPluginSelectionChanged);

    // Connect buttons
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this,
            &qmdiConfigDialog::cancelConfiguration);
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this,
            &qmdiConfigDialog::accept); // Close with Accept status
}

qmdiConfigDialog::~qmdiConfigDialog() {}

void qmdiConfigDialog::updateWidgetsForPlugin(const QString &pluginName) {
    const qmdiPluginConfig *pluginConfig = globalConfig->getPluginConfig(pluginName);
    if (pluginConfig) {
        createWidgetsFromConfig(pluginConfig);
    }
}

void qmdiConfigDialog::createWidgetsFromConfig(const qmdiPluginConfig *pluginConfig) {
    // Clear existing widgets in layout
    QLayoutItem *item;
    while ((item = configLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

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
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
        } else if (item.type == "float") {
            widget = new QDoubleSpinBox(this);
            static_cast<QDoubleSpinBox *>(widget)->setValue(item.value.toDouble());
        }

        if (widget) {
            configLayout->addWidget(widget);
        }
    }
}

void qmdiConfigDialog::onPluginSelectionChanged(const QModelIndex &index) {
    QString selectedPlugin = pluginModel->data(index, Qt::DisplayRole).toString();
    updateWidgetsForPlugin(selectedPlugin);
}

void qmdiConfigDialog::cancelConfiguration() {
    reject(); // Close the dialog with Cancel status
}
