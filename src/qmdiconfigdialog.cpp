/**
 * \file qmdiconfigdialog.cpp
 * \brief Implementation config dialog
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#include "qmdiconfigdialog.h"
#include "qmdiglobalconfig.h"
#include "qmdipluginconfig.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QWidget>

class StringListWidget : public QWidget {

  public:
    StringListWidget(QWidget *parent = nullptr) : QWidget(parent) {
        comboBox = new QComboBox(this);
        addButton = new QPushButton("Add", this);
        deleteButton = new QPushButton("Delete", this);
        modifyButton = new QPushButton("Modify", this);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QHBoxLayout *buttonLayout = new QHBoxLayout();

        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addWidget(modifyButton);

        mainLayout->addWidget(comboBox);
        mainLayout->addLayout(buttonLayout);

        mainLayout->setContentsMargins(0, mainLayout->contentsMargins().top(), 0,
                                       mainLayout->contentsMargins().bottom());
        buttonLayout->setContentsMargins(0, buttonLayout->contentsMargins().top(), 0,
                                         buttonLayout->contentsMargins().bottom());

        connect(addButton, &QPushButton::clicked, this, &StringListWidget::addItem);
        connect(deleteButton, &QPushButton::clicked, this, &StringListWidget::deleteItem);
        connect(modifyButton, &QPushButton::clicked, this, &StringListWidget::modifyItem);
    }

    void setList(const QStringList &items) {
        comboBox->clear();
        comboBox->addItems(items);
    }

    QStringList getList() {
        QStringList items;
        for (int i = 0; i < comboBox->count(); ++i) {
            items.append(comboBox->itemText(i));
        }
        return items;
    }

  private slots:
    void addItem() {
        bool ok;
        QString text = QInputDialog::getText(this, "Add Item", "Item:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) {
            comboBox->addItem(text);
        }
    }

    void deleteItem() {
        int index = comboBox->currentIndex();
        if (index != -1) {
            comboBox->removeItem(index);
        } else {
            QMessageBox::warning(this, "Delete Item", "Please select an item to delete.");
        }
    }

    void modifyItem() {
        int index = comboBox->currentIndex();
        if (index == -1) {
            return;
        }
        bool ok;
        QString text = QInputDialog::getText(this, "Modify Item", "Item:", QLineEdit::Normal,
                                             comboBox->itemText(index), &ok);
        if (ok && !text.isEmpty()) {
            comboBox->setItemText(index, text);
        }
    }

  private:
    QComboBox *comboBox;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *modifyButton;
};

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
    configAndButtonsLayout->setContentsMargins(0, 0, 0, 0);

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
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
        QLabel *label = nullptr;
        QWidget *widget = nullptr;

        if (!item.userEditable) {
            continue;
        }

        switch (item.type) {
        case qmdiConfigItem::String:
            label = new QLabel(item.displayName, this);
            widget = new QLineEdit(this);
            static_cast<QLineEdit *>(widget)->setText(item.value.toString());
            break;
        case qmdiConfigItem::Bool:
            widget = new QCheckBox(this);
            static_cast<QCheckBox *>(widget)->setChecked(item.value.toBool());
            static_cast<QCheckBox *>(widget)->setText(item.displayName);
            break;
        case qmdiConfigItem::Int8:
            label = new QLabel(item.displayName, this);
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(INT8_MIN);
            static_cast<QSpinBox *>(widget)->setMaximum(INT8_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::Int16:
            label = new QLabel(item.displayName, this);
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(INT16_MIN);
            static_cast<QSpinBox *>(widget)->setMaximum(INT16_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::Int32:
            label = new QLabel(item.displayName, this);
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(INT32_MIN);
            static_cast<QSpinBox *>(widget)->setMaximum(INT32_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::UInt8:
            label = new QLabel(item.displayName, this);
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(0);
            static_cast<QSpinBox *>(widget)->setMaximum(UINT8_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::UInt16:
            label = new QLabel(item.displayName, this);
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(0);
            static_cast<QSpinBox *>(widget)->setMaximum(UINT16_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::UInt32:
            label = new QLabel(item.displayName, this);
            widget = new QSpinBox(this);
            static_cast<QSpinBox *>(widget)->setMinimum(0);
            static_cast<QSpinBox *>(widget)->setMaximum(UINT32_MAX);
            static_cast<QSpinBox *>(widget)->setValue(item.value.toInt());
            break;
        case qmdiConfigItem::Float:
            label = new QLabel(item.displayName, this);
            widget = new QDoubleSpinBox(this);
            static_cast<QDoubleSpinBox *>(widget)->setValue(item.value.toFloat());
            break;
        case qmdiConfigItem::Double:
            label = new QLabel(item.displayName, this);
            widget = new QDoubleSpinBox(this);
            static_cast<QDoubleSpinBox *>(widget)->setValue(item.value.toDouble());
            break;
        case qmdiConfigItem::StringList:
            label = new QLabel(item.displayName, this);
            widget = new StringListWidget(this);
            static_cast<StringListWidget *>(widget)->setList(item.value.toStringList());
            break;
        case qmdiConfigItem::OneOf:
            label = new QLabel(item.displayName, this);
            widget = new QComboBox(this);
            static_cast<QComboBox *>(widget)->addItems(item.possibleValue.toStringList());
            static_cast<QComboBox *>(widget)->setCurrentIndex(item.value.toInt());
            break;
        }

        if (label) {
            configLayout->addWidget(label);
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
            if (!configItem.userEditable) {
                continue;
            }
            QWidget *widget = widgetMap[configItem.key];

            switch (configItem.type) {
            case qmdiConfigItem::String:
                break;
                configItem.value = static_cast<QLineEdit *>(widget)->text();
            case qmdiConfigItem::Bool:
                configItem.value = static_cast<QCheckBox *>(widget)->isChecked();
                break;
            case qmdiConfigItem::Int8:
            case qmdiConfigItem::Int16:
            case qmdiConfigItem::Int32:
                configItem.value = static_cast<QSpinBox *>(widget)->value();
                break;
            case qmdiConfigItem::UInt8:
            case qmdiConfigItem::UInt16:
            case qmdiConfigItem::UInt32:
                configItem.value = static_cast<QSpinBox *>(widget)->value();
                break;
            case qmdiConfigItem::Float:
            case qmdiConfigItem::Double:
                configItem.value = static_cast<QDoubleSpinBox *>(widget)->value();
                break;
            case qmdiConfigItem::StringList:
                configItem.value = static_cast<StringListWidget *>(widget)->getList();
                break;
            case qmdiConfigItem::OneOf:
                configItem.value = static_cast<QComboBox *>(widget)->currentIndex();
                break;
            }
        }
    }

    accept();
}
