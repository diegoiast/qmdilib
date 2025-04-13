/**
 * \file qmdiconfigdialog.h
 * \brief Definition config dialog
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * SPDX-License-Identifier: LGPL 2 or 3
 * \see qmdiActionGroup
 */

#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMap>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QWidget>

class qmdiGlobalConfig;
class qmdiPluginConfig;
struct qmdiConfigItem;
class PluginManager;

/**
 * @brief A dialog which can modify a global configuration
 *
 * This class lets the user of your application the ability to modify configuraton.
 *
 * This class is stand alone, and its up to you, the developer, to intergrate it into
 * you application, and adapt to the changes on the config (or even saving to disk).
 *
 * Seee \file main3.cpp as an example of usage.
 */
class qmdiConfigDialog : public QDialog {
    Q_OBJECT

  public:
    explicit qmdiConfigDialog(qmdiGlobalConfig *globalConfig, QWidget *parent = nullptr);
    ~qmdiConfigDialog();

    void updateWidgetValues();
  signals:
    void configurationUpdated();

  private slots:
    void onPluginSelectionChanged(const QModelIndex &index);
    void cancelConfiguration();
    void acceptChanges();

  protected:
    bool eventFilter(QObject *, QEvent *) override;
    void updateWidgetsForPlugin(const QString &pluginName);
    void createWidgetsFromConfig(const qmdiPluginConfig *pluginConfig);

    QVBoxLayout *configLayout;
    QHBoxLayout *mainLayout;
    QListView *pluginListView;
    QStringListModel *pluginModel;
    QDialogButtonBox *buttonBox;

    qmdiGlobalConfig *globalConfig;
    QMap<QString, QWidget *> widgetMap;
};
