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

class qmdiHost;
class qmdiServer;
class qmdiTabWidget;

class QPopupMenu;
class QSettings;
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
    int tabForFileName(const QString &fileName) const;
    qmdiClient *clientForFileName(const QString &fileName) const;
    void setNativeSettingsManager(const QString &organization = {},
                                  const QString &application = {});
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    void setFileSettingsManager(const QString &fileName);

    void removeBuiltinActions();
    void addBuiltinActions();
    void restoreSettings();
    void saveSettings();
    void updateActionsStatus();
    bool openFile(const QString &fileName, int x = -1, int y = -1, int z = -1);
    bool openFiles(const QStringList &fileNames);

  public:
    QDockWidget *createNewPanel(Panels p, const QString &name, const QString &title,
                                QWidget *widget);
    void hidePanels(Qt::DockWidgetArea area);
    void showPanels(Qt::DockWidgetArea area);
    qmdiClient *currentClient() const;
    void replaceMdiServer(qmdiServer *newServer);

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
    qmdiClient *getMdiClient(int i) const;

    void loadConfig(const QString &fileName);

  signals:
    void configurationUpdated();
    void newFileRequested();
    void newClientAdded(qmdiClient *);

  protected:
    void initGUI();

    Ui::PluginManagedWindow *ui;
    QSettings *settingsManager;
    ClosedDocuments closedDocuments;
    QMenu *closedDocumentsMenu;

    QAction *actionNewFile;
    QAction *actionOpen;
    QAction *actionClose;
    QAction *actionQuit;
    QAction *actionConfig;
    QAction *actionNextTab;
    QAction *actionPrevTab;
    QAction *actionHideGUI;

    QList<IPlugin *> plugins;
    qmdiServer *mdiServer;
    qmdiGlobalConfig config;
};
