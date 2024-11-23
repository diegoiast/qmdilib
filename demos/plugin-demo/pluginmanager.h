#pragma once

/**
 * \file pluginmanager.h
 * \brief Definition of the PluginManager class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see PluginManager
 */

#include "qmdiglobalconfig.h"
#include "qmdihost.h"
#include <QMainWindow>

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

#include <QDebug>
#include <QList>
#include <QMenu>
#include <QQueue>
#include <QString>

class ClosedDocuments {
  public:
    ClosedDocuments(int maxSize = 10) : maxSize(maxSize) {}

    // Add a document to the closed list
    void push(const QString &docName) {
        if (closedDocuments.size() >= maxSize) {
            closedDocuments.dequeue(); // Remove the oldest document
        }
        closedDocuments.enqueue(docName);
    }

    // Pop the last closed document
    QString pop() {
        if (closedDocuments.isEmpty()) {
            return QString(); // Empty queue case
        }
        return closedDocuments.dequeue();
    }

    // Peek the next 'n' documents (default 5)
    QList<QString> peekNext(int count = 5) {
        QList<QString> nextItems;
        int availableCount = qMin(count, closedDocuments.size());
        for (int i = 0; i < availableCount; ++i) {
            nextItems.append(closedDocuments[i]);
        }
        return nextItems;
    }

    // Check if there are closed documents
    bool hasDocuments() const { return !closedDocuments.isEmpty(); }

    void updateMenu(QMenu *menu, int count = 5) {
        menu->clear();
        QList<QString> nextItems = peekNext(count); // Get the next documents
        for (const QString &doc : nextItems) {
            qDebug() << "Adding  " << doc;
            QAction *action = new QAction(doc, menu);
            menu->addAction(action); // Add each document as an action in the menu
        }
    }

  private:
    QQueue<QString> closedDocuments;
    int maxSize; // Maximum number of documents to store
};

struct PanelState {
    bool isMinimized = false;
    QTabWidget *panel;
};

class PluginManager : public QMainWindow, public qmdiHost {
    Q_OBJECT
    friend class PluginModel;
    friend class ConfigDialog;

  public:
    PluginManager();
    virtual ~PluginManager();
    int tabForFileName(QString fileName);
    void setNativeSettingsManager(const QString &organization = QString(),
                                  const QString &application = QString());
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    void setFileSettingsManager(const QString &fileName);

    void restoreSettings();
    void saveSettings();
    void updateActionsStatus();
    bool openFile(QString fileName, int x = -1, int y = -1, int z = -1);
    bool openFiles(QStringList fileNames);

    void hideUnusedPanels();
    void hidePanel(Panels p);
    void showPanel(Panels p, int index);

  public:
    int createNewPanel(Panels p, QString name, QWidget *widget);
    QWidget *getPanel(Panels p, int index);
    qmdiClient *currentClient();

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
    PanelState westState, eastState, southState;

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
