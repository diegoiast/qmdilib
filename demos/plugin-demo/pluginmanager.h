#pragma once

/**
 * \file pluginmanager.h
 * \brief Definition of the PluginManager class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see PluginManager
 */

#include <QMainWindow>
#include <QQueue>

#include "qmdiglobalconfig.h"
#include "qmdihost.h"

class QPopupMenu;
class QSettings;
class qmdiHost;
class qmdiTabWidget;
class IPlugin;
class ConfigDialog;
class PluginModel;

namespace Ui {
class PluginManagedWindow;
}

enum class Panels { West, East, South };

class PluginManager;

class ClosedDocuments {
  public:
    ClosedDocuments(int maxSize = 10) : maxSize(maxSize) {}

    void push(const QString &docName);
    QString pop();
    void remove(const QString &doc);
    QList<QString> peekNext(int count = 5);
    inline bool hasDocuments() const { return !closedDocuments.isEmpty(); }
    void updateMenu(PluginManager *manager, QMenu *menu, int count = 5);

  private:
    QQueue<QString> closedDocuments;
    int maxSize;
};

class PluginManager : public QMainWindow, public qmdiHost {
    Q_OBJECT
    friend class PluginModel;
    friend class ConfigDialog;

  public:
    PluginManager();
    virtual ~PluginManager();
    int tabForFileName(const QString &fileName);
    void setNativeSettingsManager(const QString &organization = QString(),
                                  const QString &application = QString());
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    void setFileSettingsManager(const QString &fileName);

    void restoreSettings();
    void saveSettings();
    void updateActionsStatus();
    bool openFile(const QString &fileName, int x = -1, int y = -1, int z = -1);
    bool openFiles(const QStringList &fileNames);

  public:
    QDockWidget *createNewPanel(Panels p, const QString &name, const QString &title, QWidget *widget);
    void hidePanels(Qt::DockWidgetArea area);
    void showPanels(Qt::DockWidgetArea area);
    qmdiClient *currentClient();
    
    virtual void onClientClosed(qmdiClient *client) override;
    
  public slots:
    void addPlugin(IPlugin *newplugin);
    void removePlugin(IPlugin *oldplugin);
    void enablePlugin(IPlugin *plugin);
    void disablePlugin(IPlugin *plugin);
    void closeClient();
    void focusCenter();

    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    void on_actionConfigure_triggered();
    void on_actionQuit_triggered();
    void on_actionPrev_triggered();
    void on_actionNext_triggered();
    void on_actionHideGUI_changed();

    size_t visibleTabs() const;

    void loadConfig(const QString &fileName);

  signals:
    void configurationUpdated();
    void newFileRequested();

  protected:
    void initGUI();
    QList<IPlugin *> plugins;
    qmdiTabWidget *tabWidget;
    qmdiGlobalConfig config;
    QSettings *settingsManager;
    Ui::PluginManagedWindow *ui;

    ClosedDocuments closedDocuments;
    QMenu *closedDocumentsMenu;

    // public:
    QAction *actionNewFile;
    QAction *actionOpen;
    QAction *actionClose;
    QAction *actionQuit;
    QAction *actionConfig;
    QAction *actionNextTab;
    QAction *actionPrevTab;
    QAction *actionHideGUI;
};
