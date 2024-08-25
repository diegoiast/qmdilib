#pragma once

#include <QDialog>

class qmdiGlobalConfig;
class qmdiPluginConfig;

class QDialogButtonBox;
class QVBoxLayout;
class QHBoxLayout;
class QStringListModel;
class QListView;
class QLabel;

class qmdiConfigDialog : public QDialog {
    Q_OBJECT

  public:
    explicit qmdiConfigDialog(qmdiGlobalConfig *globalConfig, QWidget *parent = nullptr);
    ~qmdiConfigDialog();

  private slots:
    void onPluginSelectionChanged(const QModelIndex &index);
    void cancelConfiguration(); // No save method here

  private:
    void updateWidgetsForPlugin(const QString &pluginName);
    void createWidgetsFromConfig(const qmdiPluginConfig *pluginConfig);

    QVBoxLayout *configLayout;
    QHBoxLayout *mainLayout;
    QListView *pluginListView;
    QStringListModel *pluginModel;
    QDialogButtonBox *buttonBox;
    qmdiGlobalConfig *globalConfig; // Ensure this is declared correctly
};
