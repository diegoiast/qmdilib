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
class qmdiConfigItem;

class qmdiConfigDialog : public QDialog {
    Q_OBJECT

  public:
    explicit qmdiConfigDialog(qmdiGlobalConfig *globalConfig, QWidget *parent = nullptr);
    ~qmdiConfigDialog();

  private slots:
    void onPluginSelectionChanged(const QModelIndex &index);
    void cancelConfiguration();
    void acceptChanges();

  private:
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
