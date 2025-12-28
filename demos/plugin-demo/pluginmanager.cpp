/**
 * \file pluginmanager.cpp
 * \brief Implementation of the PluginManager class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see PluginManager
 */

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QString>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <QtConcurrent>

#include <qmdiconfigdialog.h>
#include <qmdiglobalconfig.h>
#include <qmdihost.h>
#include <qmdipluginconfig.h>
#include <qmdiserver.h>
#include <qmditabwidget.h>

#include "iplugin.h"
#include "pluginmanager.h"
#include "ui_pluginwindow.h"

void ClosedDocuments::push(const QString &docName) {
    closedDocuments.prepend(docName);
    if (closedDocuments.size() > maxSize) {
        closedDocuments.removeLast();
    }
}

QString ClosedDocuments::pop() {
    if (closedDocuments.isEmpty()) {
        return {};
    }
    return closedDocuments.takeFirst();
}

void ClosedDocuments::remove(const QString &doc) { closedDocuments.removeAll(doc); }

QList<QString> ClosedDocuments::peekNext(int count) {
    QList<QString> nextItems;
    auto availableCount = qMin(count, closedDocuments.size());
    for (auto i = 0; i < availableCount; ++i) {
        nextItems.append(closedDocuments[i]);
    }
    return nextItems;
}

void ClosedDocuments::updateMenu(PluginManager *manager, QMenu *menu, int count) {
    for (auto &a : menu->actions()) {
        manager->removeAction(a);
    }
    menu->clear();
    auto nextItems = peekNext(count);
    for (auto i = 0; i < nextItems.size(); ++i) {
        auto &doc = nextItems[i];
        QAction *action;
        if (i < 10) {
            // for the first 10 items add a shortcut
            action = new QAction(QString("&%1 %2").arg(i + 1).arg(doc), menu);
        } else {
            action = new QAction(QString("%1 %2").arg(i + 1).arg(doc), menu);
        }
        if (i == 0) {
            action->setShortcut(QKeySequence(Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_T));
            manager->addAction(action);
        }
        QObject::connect(action, &QAction::triggered, action, [this, doc, menu, manager, count]() {
            this->remove(doc);
            this->updateMenu(manager, menu, count);
            manager->openFile(doc);
        });
        menu->addAction(action);
    }
}

void ClosedDocuments::setAllDocuments(const QStringList &newList) {
    closedDocuments.clear();
    for (const QString &file : newList) {
        closedDocuments.append(file);
    }
}

/**
 * \class PluginManager
 * \brief A class which manages a list of plugins and merges their menus and
 * toolbars to the main application \author Diego Iastrubni
 * (diegoiast@gmail.com)
 *
 * The plugin manager is a main window, and can save and restore it's state if a
 * settings manager has been defined. A settings manager (and instace of
 * QSettings) can be set to work on a local file (an ini file).
 *
 * The plugin manager is a mdi host (qmdiHost) which can load menus and toolbars
 * from a the selected mdi client mdi server, and also maintain another set of
 * mdi clients, which have no real GUI, but their menus and toolbars are merged
 * to the main host. This can be used to enable or disable functionality of the
 * application on run time - by enabling or disabling IPlugin objetcs.
 *
 * Each plugin has defines a set of menus and toolbars, and has some methods
 * for saving and restoring it's state. The menus and toolbars will create
 * actions visible on the main GUI which will trigger actions on the plugin.
 * Then the plugin can ask the mdi host to add a new mdi client.
 *
 * Plugins have also the concept of "new files", a set of commands which create
 * new files and an mdi client. Plugins also define the set of files which can
 * be opened, and can respond to requests to open a specific file.
 *
 * The plugin manager can also open a dialog to configure all the plugins
 * available, and enable or disable several plugins.
 *
 * A typical usage of this class will be:
 *
 * \code
 * QApplication app( argc, argv );
 * PluginManager pluginManager;
 * pluginManager.setFileSettingsManager( "settings.ini" );
 *
 * // load a few plugins
 * pluginManager.addPlugin( new ... );
 * pluginManager.addPlugin( new ... );
 * pluginManager.updateGUI();
 *
 * // start the application
 * pluginManager.restoreSettings();
 * return app.exec();
 * \endcode
 */

/**
 * \var PluginManager::plugins
 * \brief the list of plugins
 *
 * This is the list of plugins available on the system. It contains all plugins,
 * even ones disabled. When a plugin is added, it's appended to this list.
 */

/**
 * \var PluginManager::tabWidget
 * \brief the main widget of the form
 *
 * The plugin manager is built arround a QMainWindow, which it's main widget is
 * this tab widget. This serves also as the qmdiServer.
 *
 * \see qmdiServer
 */

/**
 * \var PluginManager::newFilePopup
 * \brief the "File/New" submenu
 *
 * This is the popup menu seen by the user when on the \b File menu.
 *
 * \see newFileActions()
 */

/**
 * \var PluginManager::actionOpen
 * \brief the open action
 *
 * This action is the one added to the \b File menu, as the \b Open... command.
 *
 * \see on_actionOpen_triggered()
 */

/**
 * \var PluginManager::actionClose
 * \brief the close action
 *
 * This action is the one added to the \b File menu, as the \b Close command.
 *
 * \see on_actionClose_triggered()
 */

/**
 * \var PluginManager::actionQuit
 * \brief the quit action
 *
 * This action is the one added to the \b File meun, as the quit command.
 *
 * \see on_actionQuit_triggered()
 */

/**
 * \var PluginManager::actionConfig
 * \brief the configuration action
 *
 * This action is the one added to the \b Settings menu, as the \b Configure
 * command.
 *
 * \see on_actionConfigure_triggered()
 */

/**
 * \var PluginManager::actionNextTab
 * \brief select the next tab action
 *
 * This action is the one added to the \b Settings menu, as the \b Next \b Tab
 * command.
 *
 * \see on_actionNext_triggered()
 */

/**
 * \var PluginManager::actionPrevTab
 * \brief select the previous tab action
 *
 * This action is the one added to the \b Settings menu, as the \b Previous
 * \b Tab command.
 *
 * \see on_actionPrev_triggered()
 */

/**
 * \var PluginManager::configDialog
 * \brief the configuration dialog
 *
 * An instace to the configuration dialog.
 */

/**
 * \var PluginManager::settingsManager
 * \brief the settings manager
 *
 * A simple pointer to a QSettings variable.
 */

auto static parseFilename(const QString &input) -> std::tuple<QString, qmdiClientState> {
    QString path;
    qmdiClientState state;

    auto hashIndex = input.indexOf('#');
    if (hashIndex == -1) {
        return {input, state};
    }

    path = input.left(hashIndex);
    auto paramString = input.mid(hashIndex + 1);
    auto pairs = paramString.split(',', Qt::SkipEmptyParts);
    for (auto const &pair : std::as_const(pairs)) {
        auto equalIndex = pair.indexOf('=');
        if (equalIndex != -1) {
            QVariant value;
            auto key = pair.left(equalIndex).trimmed();
            auto valueStr = pair.mid(equalIndex + 1).trimmed();
            auto ok = false;
            auto intValue = valueStr.toInt(&ok);

            if (ok) {
                value = intValue;
            } else {
                auto doubleValue = valueStr.toDouble(&ok);
                if (ok) {
                    value = doubleValue;
                } else {
                    value = valueStr;
                }
            }
            state.insert(key, value);
        }
    }
    return {path, state};
}

auto static encodeFileName(const QString &filename, const qmdiClientState &state) -> QString {
    if (state.isEmpty()) {
        return filename;
    }

    QStringList paramList;
    for (auto it = state.constBegin(); it != state.constEnd(); ++it) {
        auto key = it.key();
        auto value = it.value().toString();
        paramList.append(QString("%1=%2").arg(key, value));
    }
    auto paramString = paramList.join(',');
    return QString("%1#%2").arg(filename, paramString);
}

/**
 * \brief default constructor
 *
 * Builds a plugin manager. Creates the actions needed for the "File" menu,
 * the "New" sub-menu, action for loading files, moving tabs etc.
 *
 * Sets the settingsManager to nullptr, which means by default nullptrptr is no option
 * to restore the state of the application.
 *
 * The config dialog is set to nullptr - and will be created on demand.
 *
 * Eventually will call initGUI() to create the main GUI.
 *
 * \see initGUI()
 */
PluginManager::PluginManager() {
    settingsManager = nullptr;

    closedDocumentsMenu = new QMenu(this);
    closedDocumentsMenu->menuAction()->setText(tr("Closed documents..."));
    toolbarsMenu = new QMenu(this);

    showDocksAction = new QAction("&Docks", this);
    connect(showDocksAction, &QAction::triggered, this, [this]() {
        QPoint menuPos;
        if (menuBar()->isVisible()) {
            auto localPos = menuBar()->actionGeometry(showDocksAction).bottomLeft();
            menuPos = menuBar()->mapToGlobal(localPos);
        } else {
            menuPos = QCursor::pos();
        }
        toolbarsMenu->popup(menuPos);
    });

    actionNewFile = new QAction(tr("&New..."), this);
    actionOpen = new QAction(tr("&Open..."), this);
    actionClose = new QAction(tr("C&lose"), this);
    actionQuit = new QAction(tr("Ex&it"), this);
    actionConfig = new QAction(tr("&Config"), this);
    actionNextTab = new QAction(tr("&Next tab"), this);
    actionPrevTab = new QAction(tr("&Previous tab"), this);
    actionMoveTabRight = new QAction(tr("Move tab &forward"), this);
    actionMoveTabLeft = new QAction(tr("Move tab &backward"), this);
    actionHideGUI = new QAction(tr("&Hide menus"), this);

    actionNextTab->setEnabled(false);
    actionPrevTab->setEnabled(false);
    actionMoveTabRight->setEnabled(false);
    actionMoveTabLeft->setEnabled(false);
    actionClose->setEnabled(false);

    actionNewFile->setObjectName("actionNewFile");
    actionOpen->setObjectName("actionOpen");
    actionClose->setObjectName("actionClose");
    actionQuit->setObjectName("actionQuit");
    actionConfig->setObjectName("actionConfigure");
    actionNextTab->setObjectName("actionNext");
    actionPrevTab->setObjectName("actionPrev");
    actionHideGUI->setObjectName("actionHideGUI");
    actionHideGUI->setCheckable(true);

    actionConfig->setIcon(QIcon::fromTheme("configure"));
    actionConfig->setShortcut(QKeySequence::Preferences);
    if (actionConfig->icon().isNull()) {
        actionConfig->setIcon(QIcon::fromTheme("preferences-other-symbolic"));
    }
    actionQuit->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit));
    actionNextTab->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext));
    actionPrevTab->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious));

    actionNewFile->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
    actionNewFile->setShortcut(QKeySequence::New);
    connect(actionNewFile, &QAction::triggered, this,
            [this]() { emit newFileRequested(sender()); });

    actionOpen->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
    actionOpen->setShortcut(QKeySequence::Open);

    actionClose->setIcon(QIcon::fromTheme("document-close"));
    // On windows this is Control+F4, which is lame.
    // actionClose->setShortcut(QKeySequence::Close);
    actionClose->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    actionNextTab->setShortcuts({Qt::ALT | Qt::Key_Right, QKeySequence::NextChild});
    actionPrevTab->setShortcuts({Qt::ALT | Qt::Key_Left, QKeySequence::PreviousChild});
    actionMoveTabRight->setShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_Right);
    actionMoveTabLeft->setShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_Left);
    connect(actionMoveTabLeft, &QAction::triggered, this, &PluginManager::doMoveTabBackward);
    connect(actionMoveTabRight, &QAction::triggered, this, &PluginManager::doMoveTabForward);

    actionHideGUI->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_M));
    initGUI();

    for (auto i = 0; i < 8; ++i) {
        auto tabSelectShortcut = new QAction(this);
        auto key = static_cast<Qt::Key>(Qt::Key_1 + i);
        tabSelectShortcut->setShortcut(QKeySequence(Qt::AltModifier | key));
        tabSelectShortcut->setShortcutContext(Qt::ApplicationShortcut);
        connect(tabSelectShortcut, &QAction::triggered, this,
                [this, i]() { mdiServer->setCurrentClientIndex(i); });
        addAction(tabSelectShortcut);
    }

    // alt+9 will always send you to the last
    {
        auto tabSelectShortcut = new QAction(this);
        tabSelectShortcut->setShortcut(QKeySequence(Qt::AltModifier | Qt::Key_9));
        tabSelectShortcut->setShortcutContext(Qt::ApplicationShortcut);
        connect(tabSelectShortcut, &QAction::triggered, this, [this]() {
            auto size = mdiServer->getClientsCount();
            if (size > 0) {
                mdiServer->setCurrentClientIndex(size - 1);
            }
        });
        addAction(tabSelectShortcut);
    }

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::TabPosition::West);
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::TabPosition::East);
}

/**
 * \brief default destructor
 *
 * Deletes the object. If a settings manager is allocated it will be deleted as
 * well.
 *
 * \see setFileSettingsManager()
 * \see setNativeSettingsManager()
 * \see QSettings
 */
PluginManager::~PluginManager() {
    if (settingsManager) {
        saveSettings();
        delete settingsManager;
    }

    for (auto p : std::as_const(plugins)) {
        if (plugins.removeAll(p) == 1) {
            delete p;
        } else {
            qDebug("%s - %d: could not remove plugin from the plugin manager (%s)", __FILE__,
                   __LINE__, qPrintable(p->getName()));
            return;
        }
    }
    delete ui;
}

/**
 * \brief return the index of the tab in which this file is loaded
 * \param fileName the fully qualified file name to search
 * \return -1 if not found, otherwise the tab number
 *
 * This function will query all widgets in the QTabWidget of the main window
 * to see if they implement the qmdiClient interface. If a tab does implement
 * it - it will try and see if the mdiClientFileName() is the same as one passed
 * to this method.
 *
 * The method returns -1 if no mdi client is found that has loaded that file.
 * This means that if you load that file and insert it into the tab widget
 * directly, and not deriving qmdiClient this method will not see you new
 * widget.
 *
 * If the file has been found the method returns a number corresponding to the
 * tab in which the file has been loaded.
 *
 * \see openFile()
 */
int PluginManager::tabForFileName(const QString &fileName) const {
    auto url = QUrl(fileName);

    // Some plugins notify about status of files, this is not really
    // a file to be opened.
    auto scheme = url.scheme();
    auto isLocalFile = url.isLocalFile();
    if (!scheme.isEmpty() && !isLocalFile) {
        return -1;
    }
    if (fileName.isEmpty()) {
        return -1;
    }

    for (auto i = 0; i < mdiServer->getClientsCount(); i++) {
        auto c = mdiServer->getClient(i);
        if (!c) {
            continue;
        }

        auto clientName = c->mdiClientFileName();
        if (clientName == fileName) {
            return i;
        }
    }
    return -1;
}

qmdiClient *PluginManager::clientForFileName(const QString &fileName) const {
    if (fileName.isEmpty()) {
        return nullptr;
    }

    for (auto i = 0; i < mdiServer->getClientsCount(); i++) {
        auto c = mdiServer->getClient(i);
        if (c) {
            if (c->mdiClientFileName() == fileName) {
                return c;
            }
        }
    }
    return nullptr;
}

/**
 * \brief set a native settings manager to this plugin manager
 * \param organization the organization your application belongs to
 * \param application the name of your application
 *
 * This method installs a QSettings in native mode (for windows this means using
 * the registry, and on *nix this means using an INI file) to be used to save
 * and restore settings. Any settings manager available before will be deleted.
 *
 * The \b organization and \b application parameters are passed to the
 * constructor of QSettings.
 *
 * \see setFileSettingsManager()
 * \see QSettings
 */
void PluginManager::setNativeSettingsManager(const QString &organization,
                                             const QString &application) {
    if (settingsManager) {
        delete settingsManager;
    }
    settingsManager = new QSettings(organization, application);
}

void PluginManager::closeEvent(QCloseEvent *event) {
    for (auto i = 0; i < mdiServer->getClientsCount(); i++) {
        auto client = mdiServer->getClient(i);
        if (!client) {
            continue;
        }
        if (!client->canCloseClient(CloseReason::ApplicationQuit)) {
            event->ignore();
            return;
        }
    }

    event->accept();
}

/**
 * \brief set an ini based settings manager to this plugin manaher
 * \param fileName the file to store the settings
 *
 * This method installs a QSettings in IniFormat mode (which means on all
 * platforms an INI file will be used) to be used to save and restore settings.
 * Any settings manager available before will be deleted.
 *
 * The \b fileName is the file to use as the backend. For more information
 * read the manual of QSettings.
 *
 * \see setNativeSettingsManager()
 * \see QSettings
 */
void PluginManager::setFileSettingsManager(const QString &fileName) {
    if (settingsManager) {
        delete settingsManager;
    }
    settingsManager = new QSettings(fileName, QSettings::IniFormat);
}

void PluginManager::removeBuiltinActions() {
    menus.removeActionsFromWidget(this);
    toolbars.removeActionsFromWidget(this);
}

void PluginManager::addBuiltinActions() {
    menus.addActionsToWidget(this);
    toolbars.addActionsToWidget(this);
}

/**
 * \brief restore the state of the application
 *
 * Calling this method will restore the status of the main window
 * (size, position etc), and all loaded documents.
 *
 * Before loading the documents the plugin manager will ask all loaded plugins
 * to restore their state by calling IPlugin::loadConfig(). At the end this
 * method will also call updateActionsStatus()
 *
 * This method does nothing if no setting manager has been defined.
 *
 * \note When restoring the loaded documents, it may be possible to load a
 * document using a different plugin, if a "more suitable plugin" is available
 * when restoring the application state.
 *
 * \see IPlugin::loadConfig()
 * \see IPlugin::canOpenFile()
 * \see saveSettings()
 * \see updateActionsStatus()
 */
void PluginManager::restoreSettings() {
    // TODO - no idea why I need to do this. Sometimes, the docking areas get borked
    //        unless I re-do this now (first time is in the constructor).
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::TabPosition::West);
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::TabPosition::East);

    if (!settingsManager) {
        return;
    }

    // TODO - use the global config API

    // main window state
    settingsManager->beginGroup("mainwindow");
    {
        restoreState(settingsManager->value("state").toByteArray());
        restoreGeometry(settingsManager->value("geometry").toByteArray());
        if (settingsManager->contains("hidegui")) {
            auto status = settingsManager->value("hidegui").toBool();
            actionHideGUI->setChecked(status);
        }
    }
    settingsManager->endGroup();

    for (auto &plugin : plugins) {
        plugin->loadConfig(*settingsManager);
    }

    // restore opened files
    settingsManager->beginGroup("files");
    {
        for (auto &s : settingsManager->childKeys()) {
            if (!s.startsWith("file")) {
                continue;
            }
            auto fileNameDetails = settingsManager->value(s).toString();
            auto [fileName, state] = parseFilename(fileNameDetails);
            if (auto c = openFile(fileName)) {
                c->setState(state);
            }
        }

        // re-select the current tab
        auto current = settingsManager->value("current", -1).toInt();
        if (current != -1) {
            mdiServer->setCurrentClientIndex(current);
        }

        auto allClosedDocuments = settingsManager->value("closed").toStringList();
        closedDocuments.setAllDocuments(allClosedDocuments);
        closedDocuments.updateMenu(this, closedDocumentsMenu);
    }
    settingsManager->endGroup();
}

/**
 * \brief save the state of the application into the settings manager
 *
 * This method stores the state of the window (size, position, etc) to the
 * settings manager. It will save the list of qmdiClients available on the tab
 * widget. If in one of the tabs nullptrptr is a non mdi client instead of the
 * filename, in that the configuration file will save "@".
 *
 * This method will also call each one of the plugins and ask them to store
 * their configuration. Each plugin will have it's own section, named with the
 * plugin's name.
 *
 * This method will also sync the settings file.
 *
 * \see restoreSettings()
 * \see IPlugin::saveConfig()
 * \see QSettings::sync()
 */
void PluginManager::saveSettings() {
    if (!settingsManager) {
        return;
    }

    // TODO - port to the plugin config system

    // main window state
    settingsManager->beginGroup("mainwindow");
    {
        settingsManager->setValue("state", saveState());
        settingsManager->setValue("geometry", saveGeometry());
        settingsManager->setValue("hidegui", actionHideGUI->isChecked());
    }
    settingsManager->endGroup();

    // store saved opened files
    settingsManager->remove("files");
    settingsManager->beginGroup("files");
    if (mdiServer->getClientsCount() != 0) {
        qmdiClient *c = nullptr;
        auto s = QString{};
        for (auto i = 0; i < mdiServer->getClientsCount(); i++) {
            c = mdiServer->getClient(i);
            if (!c) {
                continue;
            }

            auto savedFileName = c->mdiClientFileName();
            auto state = c->getState();
            auto valueName = QString("file%1").arg(i);
            auto fullValue = encodeFileName(savedFileName, state);
            settingsManager->setValue(valueName, fullValue);
        }
        settingsManager->setValue("current", mdiServer->getCurrentClientIndex());
        settingsManager->setValue("closed", closedDocuments.getAllDocuments());
    }
    settingsManager->endGroup();

    // let each ones of the plugins save it's state
    for (auto &p : plugins) {
        p->saveConfig(*settingsManager);
    }

    settingsManager->sync();
}

/**
 * \brief update some actions
 *
 * Calling this method will update the next/prev/close commands of the main
 * window.
 */
void PluginManager::updateActionsStatus() {
    auto widgetsCount = mdiServer->getClientsCount();
    actionClose->setEnabled(widgetsCount != 0);
    actionNextTab->setEnabled(widgetsCount > 1);
    actionPrevTab->setEnabled(widgetsCount > 1);
    actionMoveTabLeft->setEnabled(widgetsCount > 1);
    actionMoveTabRight->setEnabled(widgetsCount > 1);
}

/**
 * \brief open a file, using the most suitable plugin
 * \param fileName a file name, or some king of URL
 * \param x dummy variable - see documentation of IPlugin::openFile()
 * \param y dummy variable - see documentation of IPlugin::openFile()
 * \param z dummy variable - see documentation of IPlugin::openFile()
 * \return false if the loading failed from any reason
 *
 * The plugin manager will try to find a plugin which is most suitable for
 * loading this file, by calling IPlugin::canOpenFile(). The plugin which
 * returns the highest score will be selected to open this file.
 *
 * \note It's not necesary for \b fileName to be a real file. It is possible to
 * ask for "loading" some "file", which can have any meaning you define in the
 * plugin handling this kind of "document".
 *
 * The parameters \b x \b y \b z will be sent to IPlugin::openFile(). Follow
 * the documentation for more details.
 *
 * \see IPlugin::canOpenFile()
 * \see IPlugin::openFile()
 * \todo how does a developer know why the loading of a file failed?
 */
qmdiClient *PluginManager::openFile(const QString &fileName, int x, int y, int z) {
    // see which plugin is the most suited for openning this file
    IPlugin *bestPlugin = nullptr;
    auto highestScore = -1;
    for (auto &p : plugins) {
        if (!p->enabled) {
            continue;
        }

        // is this plugin better then the selected?
        auto i = p->canOpenFile(fileName);
        if (i > highestScore) {
            bestPlugin = p;
            highestScore = i; // bestPlugin->canOpenFile(fileName);
        }
    }

    if (!bestPlugin) {
        // no plugin can handle this file,
        // this should not happen, and usually means a bug
        return nullptr;
    }

    auto i = tabForFileName(fileName);
    // see if it's already open
    if (i != -1) {
        auto client = mdiServer->getClient(i);
        mdiServer->setCurrentClientIndex(i);
        return bestPlugin->navigateFile(client, x, y, x);
    }

    // ask best plugin to open the file
    auto fileOpened = bestPlugin->openFile(fileName, x, y, z);
    return fileOpened;
}

/**
 * \brief open a list of files
 * \param fileNames a list of files to load
 * \return false if any one of the file loadings failed
 *
 * Each one of the files will be opened by the most suitable plugin in the
 * system. The files will be opened with the default parameters (x=-1, y=-1,
 * z=-1).
 *
 * \see openFile()
 * \todo how does a developer know why the loading of one of the files failed?
 */
bool PluginManager::openFiles(const QStringList &fileNames) {
    auto b = true;
    for (auto const &s : fileNames) {
        b = b && openFile(s);
        QApplication::processEvents();
    }

    return b;
}

CommandArgs PluginManager::handleCommand(const QString &command, const CommandArgs &args) {
    IPlugin *bestPlugin = nullptr;
    auto highestScore = 0;

    for (auto &p : plugins) {
        if (!p->enabled) {
            continue;
        }
        auto i = p->canHandleCommand(command, args);
        if (i > highestScore) {
            bestPlugin = p;
            highestScore = i;
        }
    }

    if (bestPlugin) {
        auto result = bestPlugin->handleCommand(command, args);
        return result;
    }
    return {};
}

QFuture<CommandArgs> PluginManager::handleCommandAsync(const QString &command, const CommandArgs &args) {
    // Find the best plugin that can handle this command
    IPlugin *bestPlugin = nullptr;
    auto highestScore = 0;

    for (auto &p : plugins) {
        if (!p->isEnabled()) {
            continue;
        }

        // Check if the plugin can handle this command asynchronously
        int asyncScore = p->canHandleAsyncCommand(command, args);
        if (asyncScore > highestScore) {
            bestPlugin = p;
            highestScore = asyncScore;
        }
    }

    // If we found a plugin that supports async for this command, use it
    if (bestPlugin) {
        return bestPlugin->handleCommandAsync(command, args);
    }

    // Fall back to running the sync version in a QtConcurrent thread
    return QtConcurrent::run([this, command, args]() -> CommandArgs {
        return handleCommand(command, args);
    });
}

auto static findFirstDockWidget(QMainWindow *mainWindow, Qt::DockWidgetArea dockArea)
    -> QDockWidget * {
    for (auto &widget : mainWindow->findChildren<QWidget *>()) {
        if (auto dockWidget = qobject_cast<QDockWidget *>(widget)) {
            if (mainWindow->dockWidgetArea(dockWidget) == dockArea) {
                return dockWidget;
            }
        }
    }
    return nullptr;
}

QDockWidget *PluginManager::createNewPanel(Panels p, const QString &name, const QString &title,
                                           QWidget *widget) {
    auto dock = new QDockWidget(title, this);
    dock->setWidget(widget);
    dock->setObjectName(name);

    auto dockArea = Qt::NoDockWidgetArea;
    auto features = dock->features();
    dock->setFeatures(features & ~QDockWidget::DockWidgetFloatable);

    switch (p) {
    case Panels::East:
        dockArea = Qt::RightDockWidgetArea;
        break;
    case Panels::West:
        dockArea = Qt::LeftDockWidgetArea;
        break;
    case Panels::South:
        dockArea = Qt::BottomDockWidgetArea;
        break;
    default:
        return dock;
    }

    auto currentDock = findFirstDockWidget(this, dockArea);
    if (currentDock) {
        tabifyDockWidget(currentDock, dock);
    } else {
        addDockWidget(dockArea, dock);
    }
    QTimer::singleShot(0, this, &PluginManager::updateToolbarsMenu);
    return dock;
}

void PluginManager::hidePanels(Qt::DockWidgetArea area) {
    for (auto &dockWidget : findChildren<QDockWidget *>()) {
        if (dockWidgetArea(dockWidget) == area) {
            dockWidget->hide();
        }
    }
}

void PluginManager::showPanels(Qt::DockWidgetArea area) {
    for (auto &dockWidget : findChildren<QDockWidget *>()) {
        if (dockWidgetArea(dockWidget) == area) {
            dockWidget->show();
        }
    }
}

qmdiClient *PluginManager::currentClient() const { return mdiServer->getCurrentClient(); }

void PluginManager::replaceMdiServer(qmdiServer *newServer) {
    auto w = dynamic_cast<QWidget *>(newServer);
    if (!w) {
        // this is not a widget
        return;
    }

    auto oldMdiServer = mdiServer;
    setCentralWidget(w);
    mdiServer = newServer;
    mdiServer->mdiHost = this;
    mdiServer->setOnMdiSelected([this](qmdiClient *, int) { updateActionsStatus(); });

    // update the mdi server in each plugin
    for (auto p : std::as_const(plugins)) {
        p->mdiServer = newServer;
    }
    delete oldMdiServer;
}

void PluginManager::onClientClosed(qmdiClient *client) {
    if (client && !client->mdiClientName.isEmpty()) {
        closedDocuments.push(client->mdiClientFileName());
        closedDocuments.updateMenu(this, closedDocumentsMenu);
    }
}

/**
 * \brief add a new plugin to the plugin manager system
 * \param newplugin the plugin to add to the system
 *
 * This will add the plugin to the system. If a settings manager is available,
 * the new plugin will be asked to load it's configuration from the settings
 * manager.
 *
 * If the auto enabled flag of the new plugin is enabled the plugin will get
 * enabled.
 *
 * Plugins added to the plugin manager are deleted by the plugin manager when it
 * is deleted, or when the plugin is removed from the list of plugins.
 *
 * \see IPlugin::autoEnabled
 * \see IPlugin::setEnabled()
 * \see IPlugin::loadConfig()
 * \see IPlugin::newFileActions);
 */
void PluginManager::addPlugin(IPlugin *newplugin) {
    plugins << newplugin;

    if (!newplugin) {
        return;
    }

    newplugin->mdiServer = mdiServer;
    if (newplugin->alwaysEnabled) {
        newplugin->autoEnabled = true;
    }

    if (newplugin->autoEnabled) {
        newplugin->enabled = true;
    }

    if (newplugin->enabled) {
        enablePlugin(newplugin);
        if (!newplugin->config.configItems.isEmpty()) {
            config.addPluginConfig(&newplugin->config);
        }
    }

    connect(this, &PluginManager::configurationUpdated, newplugin,
            &IPlugin::configurationHasBeenModified);
}

/**
 * \brief remove a plugin from the plugin manager
 * \param oldplugin the plugin to be removed from the system
 *
 * When you call this method, any plugin passed to it will be removed from the
 * plugin manager and deleted (freeing it's memory).
 *
 * \see addPlugin
 * \see disablePlugin
 */
void PluginManager::removePlugin(IPlugin *oldplugin) {
    if (!oldplugin) {
        return;
    }

    disablePlugin(oldplugin);
    oldplugin->mdiServer = nullptr;

    if (plugins.removeAll(oldplugin) == 1) {
        delete oldplugin;
    } else {
        qDebug("%s - %d: could not remove plugin from the plugin manager (%s)", __FILE__, __LINE__,
               qPrintable(oldplugin->getName()));
        return;
    }
}

/**
 * \brief enable a plugin in the system
 * \param plugin the plugin to enable
 *
 * This method will enable a plugin and merge it's menus and actions to the main
 * gui, this will add all the add all the "new actions" to the "File/New" sub
 * menu.
 *
 * \note this method works only on plugins available in the system, please use
 * PluginManager::addPlugin() before enabling a plugin.
 *
 * \see addPlugin()
 * \see disablePlugin()
 * \see qmdiHost::mergeClient()
 */
void PluginManager::enablePlugin(IPlugin *plugin) {
    if (!plugin) {
        return;
    }

    if (!plugins.contains(plugin)) {
        qDebug("%s - %d: tried to enable a plugin which was not part of the plugin "
               "manager (%s)",
               __FILE__, __LINE__, qPrintable(plugin->getName()));
        return;
    }

    if (plugin->enabled) {
        plugin->setEnabled(true);
        mergeClient(plugin);
        plugin->menus.addActionsToWidget(this);
        plugin->toolbars.addActionsToWidget(this);
    }
}

/**
 * \brief disable a plugin in the system
 * \param plugin the plugin to disable
 *
 * This method will diable a plugin and unmerge it's menus and actions to the
 * main gui, this will add all the add all the "new actions" to the "File/New"
 * sub menu.
 *
 * \note this method works only on plugins available in the system, please use
 * PluginManager::addPlugin() before enabling a plugin.
 *
 * \see addPlugin()
 * \see enablePlugin()
 * \see qmdiHost::unmergeClient()
 */
void PluginManager::disablePlugin(IPlugin *plugin) {
    if (!plugin) {
        return;
    }

    if (!plugins.contains(plugin)) {
        qDebug("%s - %d: tried to disable a plugin which was not part of the "
               "plugin manager (%s)",
               __FILE__, __LINE__, qPrintable(plugin->getName()));
        return;
    }

    if (plugin->enabled) {
        plugin->setEnabled(false);
        unmergeClient(plugin);
    }
}

/**
 * \brief initialize the mdi client GUI
 *
 * This method initializes the main window's/mdi host GUI by creating
 * pre-defined menus and the standard actions provided by this calss.
 *
 * The actions available are:
 *  - File / New (a pop up menu, see IPlugin::newFileActions )
 *  - File / Open (see on_actionOpen_triggered() )
 *  - File / < -- this is the merge point
 *  - File / Quit
 *  - Settings / Configure (see on_actionConfigure_triggered() )
 *  - Settings / Next tab (see on_actionConfigure_triggered() )
 *  - Settings / Previous tab (see on_actionConfigure_triggered() )
 *
 * The menus generated are (in this order)
 *  - File
 *  - Edit
 *  - Search
 *  - View
 *  - Project
 *  - Build
 *  - Debug
 *  - Navigation
 *  - Settings
 *  - Window
 *  - Help
 *
 * Menus which do not contain any actions will not be displayed on screen. In
 * future versions of this library will be an option to add and remove
 * menus more freely.
 *
 * \todo add methods for adding/removing menus in a more sane way
 */
void PluginManager::initGUI() {
    menus[tr("&File")]->addAction(actionNewFile);
    menus[tr("&File")]->addAction(actionOpen);
    menus[tr("&File")]->addMenu(closedDocumentsMenu);
    menus[tr("&File")]->addSeparator();
    menus[tr("&File")]->setMergePoint();
    menus[tr("&File")]->addAction(actionClose);
    menus[tr("&File")]->addSeparator();
    menus[tr("&File")]->addAction(actionQuit);
    menus[tr("&Edit")];
    menus[tr("&Search")];
    menus[tr("&View")];
    menus[tr("&Project")];
    menus[tr("&Build")];
    menus[tr("&Debug")];
    menus[tr("&Navigation")];
    menus[tr("&Tools")];
    menus[tr("Se&ttings")]->addAction(actionConfig);
    menus[tr("Se&ttings")]->addAction(showDocksAction);
    menus[tr("Se&ttings")]->setMergePoint();
    menus[tr("Se&ttings")]->addSeparator();
    menus[tr("Se&ttings")]->addAction(actionNextTab);
    menus[tr("Se&ttings")]->addAction(actionPrevTab);
    menus[tr("Se&ttings")]->addAction(actionHideGUI);
    menus[tr("Se&ttings")]->addAction(actionMoveTabLeft);
    menus[tr("Se&ttings")]->addAction(actionMoveTabRight);
    menus[tr("&Window")];
    menus[tr("&Help")];

    toolbars[tr("main")]->addAction(actionNewFile);
    toolbars[tr("main")]->addAction(actionOpen);
    toolbars[tr("main")]->addAction(actionConfig);

    this->ui = new Ui::PluginManagedWindow;
    this->ui->setupUi(this);
    auto tabWidget = this->ui->mdiTabWidget;
    mdiServer = tabWidget;
    connect(tabWidget, &qmdiTabWidget::newClientAdded, this, &PluginManager::newClientAdded);

    auto tabCloseBtn = new QToolButton(tabWidget);
    // TODO - convert to document list
    connect(tabCloseBtn, &QAbstractButton::clicked, this, &PluginManager::closeClient);
    tabCloseBtn->setAutoRaise(true);
    tabCloseBtn->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose));
    tabWidget->setCornerWidget(tabCloseBtn, Qt::TopRightCorner);

    auto addNewMdiClient = new QToolButton(tabWidget);
    connect(addNewMdiClient, &QAbstractButton::clicked, addNewMdiClient,
            [this]() { emit newFileRequested(sender()); });
    addNewMdiClient->setAutoRaise(true);
    addNewMdiClient->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
    tabWidget->setCornerWidget(addNewMdiClient, Qt::TopLeftCorner);

    tabWidget->mdiHost = this;
    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);

    mdiServer->setOnMdiSelected([this](qmdiClient *, int) { updateActionsStatus(); });
    addBuiltinActions();
    updateGUI();
}

/**
 * \brief close the current in the main tab widget
 *
 * This method will close the current tab in the tab widget. If the current
 * widget implements qmdiClient it will call qmdiClient::closeClient() otherwise
 * it will just delete the widget by calling QObject::deleteLater()
 */
void PluginManager::closeClient() {
    if (mdiServer->getClientsCount() < 2 && mdiServer->keepSingleClient) {
        return;
    }
    auto client = mdiServer->getCurrentClient();
    if (client == nullptr) {
        if (ui->mdiTabWidget) {
            ui->mdiTabWidget->currentWidget()->deleteLater();
        }
    } else {
        if (client->closeClient(CloseReason::CloseTab)) {
            delete client;
        }
    }
}

/**
 * @brief Set the focus on the currently active tab
 *
 * Will focus the tab widget and then set the focus to the content of the
 * current tab.
 */
void PluginManager::focusCenter() {
    if (!ui->mdiTabWidget) {
        return;
    }
    ui->mdiTabWidget->setFocus();
    ui->mdiTabWidget->currentWidget()->setFocus();
}

void PluginManager::updateToolbarsMenu() {
    for (auto d : toolbarsMenu->actions()) {
        delete d;
    }
    toolbarsMenu->clear();

    auto allDocks = getAllDockWidgets();
    auto index = 0;
    for (auto dock : std::as_const(allDocks)) {
        auto action = new QAction(dock->windowTitle(), toolbarsMenu);
        action->setCheckable(true);
        action->setChecked(dock->isVisible());
        if (index < 9) {
            action->setShortcut(QKeySequence(Qt::ControlModifier | (Qt::Key_1 + index)));
            action->setShortcutContext(Qt::ApplicationShortcut);
        }
        connect(action, &QAction::triggered, this, [dock] {
            if (!dock->isVisible()) {
                dock->show();
                dock->raise();
                dock->widget()->setFocus();
                return;
            }

            if (dock->widget()->hasFocus()) {
                dock->hide();
            } else {
                dock->show();
                dock->raise();
                dock->widget()->setFocus();
            }
        });
        connect(dock, &QDockWidget::visibilityChanged, action, &QAction::setChecked);
        toolbarsMenu->addAction(action);
        addAction(action);
        dock->setToolTip(dock->windowTitle() + " " +
                         action->shortcut().toString(QKeySequence::NativeText));
        ++index;
    }
}

/**
 * \brief show the open dialog and load files
 *
 * This slot is connected to the \b triggered signal of the actionOpen. It will
 * display an open files dialog, and let the user choose which files to open and
 * eventually call openFiles().
 *
 * The extensions for the file dialog are computer from the list of plugins
 * available and enabled in the system.
 *
 * This slot is auto connected.
 *
 * \see IPlugin::extensAvailable()
 * \see openFiles()
 */
void PluginManager::on_actionOpen_triggered() {
    static QString workingDir;

    if (workingDir.isEmpty()) {
        workingDir = QDir::homePath();
    }
    QStringList filters;
    for (auto p : std::as_const(plugins)) {
        if (!p->enabled) {
            continue;
        }
        auto pluginFilters = p->myExtensions();
        for (auto &filter : pluginFilters) {
            if (filter.isEmpty()) {
                continue;
            }
            filters.append(filter);
        }
    }

    std::sort(filters.begin(), filters.end());
    auto filterString = filters.join(";;");
    auto selectedFiles = QFileDialog::getOpenFileNames(this, tr("Open File(s)"), workingDir, filterString);
    if (selectedFiles.isEmpty()) {
        return;
    }

    auto fi = QFileInfo (selectedFiles.first());
    workingDir = fi.absolutePath();
    openFiles(selectedFiles);
}

/**
 * \brief close the current widget
 *
 * This slot will close the current document, by calling
 * qmdiTabWidget::tryCloseClient()
 *
 * This slot is auto connected. This slot is triggered by the actionClose found
 * in the \b File menu.
 *
 * \see qmdiTabWidget::tryCloseClient()
 */
void PluginManager::on_actionClose_triggered() {
    mdiServer->tryCloseClient(mdiServer->getCurrentClientIndex());
}

/**
 * \brief quit the application
 *
 * Quits the application.
 *
 * This slot is auto connected. This slot is triggered by the actionQuit
 * found in the \b File menu.
 */
void PluginManager::on_actionQuit_triggered() { this->close(); }

/**
 * \brief configure the available plugins
 *
 * This will bring up the plugin configuration dialog from which the user can
 * configure the available plugins.
 *
 * This slot is auto connected. This slot is triggered by the actionConfigure
 * found in the \b Settings menu.
 */
void PluginManager::on_actionConfigure_triggered() {
    qmdiConfigDialog dialog(&config, this);
    if (dialog.exec()) {
        saveSettings();
    }
    emit configurationUpdated();
}

/**
 * \brief select the previous (left) tab
 *
 * This slot will select on the tab widget the previous (generally the left) tab
 * and activate it. This methods does not cycle trough (meaning, when you are on
 * the first tab, calling it will not move to the last tab)
 *
 * This slot is auto connected. This slot is triggered by the actionPrevTab
 * found in the \b Settings menu.
 *
 * \see on_actionNext_triggered()
 * \see PluginManager::actionPrevTab
 */
void PluginManager::on_actionPrev_triggered() {
    auto i = mdiServer->getCurrentClientIndex();
    if (i == 0) {
        return;
    }

    i--;
    mdiServer->setCurrentClientIndex(i);
}

/**
 * \brief select the next (right) tab
 *
 * This slot will select on the tab widget the next (generally the right) tab
 * and activate it. This methods does not cycle trough (meaning, when you are on
 * the laft tab, calling it will not move to the first tab)
 *
 * This slot is auto connected. This slot is triggered by the actionNextTab
 * found in the \b Settings menu.
 *
 * \see on_actionPrev_triggered()
 * \see PluginManager::actionNextTab
 */
void PluginManager::on_actionNext_triggered() {
    auto i = mdiServer->getCurrentClientIndex();
    if (i == mdiServer->getClientsCount()) {
        return;
    }

    i++;
    mdiServer->setCurrentClientIndex(i);
}

qmdiActionGroup *PluginManager::getContextMenuActions(const QString &menuId,
                                                      const QString &filePath) {
    auto* actionGroup = new qmdiActionGroup(tr("Plugin Actions for %1").arg(menuId));
    for (auto *plugin : std::as_const(plugins)) {
        if (!plugin->isEnabled()) {
            continue;
        }
        if (auto *pluginActions = plugin->getContextMenuActions(menuId, filePath)) {
            actionGroup->mergeGroup(pluginActions);
            delete pluginActions;
        }
    }
    if (actionGroup->empty()) {
        delete actionGroup;
        return nullptr;
    }
    return actionGroup;
}

void PluginManager::on_actionHideGUI_changed() {
    setUpdatesEnabled(false);

    // TODO - no idea why I need to do this. Sometimes, the docking areas get borked
    //        unless I re-do this now (first time is in the constructor).
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::TabPosition::West);
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::TabPosition::East);

    menuBar()->setVisible(!actionHideGUI->isChecked());
    for (auto b : findChildren<QToolBar *>()) {
        b->setVisible(!actionHideGUI->isChecked());
    }
    mergeClient(currentClient());
    updateGUI();

    setUpdatesEnabled(true);
    emit minimizedModeChanged(actionHideGUI->isChecked());
}

void PluginManager::doMoveTabForward()
{
    auto i = mdiServer->getCurrentClientIndex();
    mdiServer->moveClient(i, i + 1);
}

void PluginManager::doMoveTabBackward()
{
    auto i = mdiServer->getCurrentClientIndex();
    mdiServer->moveClient(i, i - 1);
}

size_t PluginManager::visibleTabs() const { return mdiServer->getClientsCount(); }

qmdiClient *PluginManager::getMdiClient(int i) const { return mdiServer->getClient(i); }

void PluginManager::loadConfig(const QString &fileName) { config.loadFromFile(fileName); }

QList<QDockWidget *> PluginManager::getAllDockWidgets() const {
    auto docks = findChildren<QDockWidget *>(QString(), Qt::FindDirectChildrenOnly);

    std::stable_sort(docks.begin(), docks.end(), [this](QDockWidget *a, QDockWidget *b) -> bool {
        auto areaA = dockWidgetArea(a);
        auto areaB = dockWidgetArea(b);

        // Global area order: Top → Left → Bottom → Right (counter-clockwise)
        auto areaOrderA = (areaA == Qt::TopDockWidgetArea)      ? 0
                          : (areaA == Qt::LeftDockWidgetArea)   ? 1
                          : (areaA == Qt::BottomDockWidgetArea) ? 2
                          : (areaA == Qt::RightDockWidgetArea)  ? 3
                                                                : 4;

        auto areaOrderB = (areaB == Qt::TopDockWidgetArea)      ? 0
                          : (areaB == Qt::LeftDockWidgetArea)   ? 1
                          : (areaB == Qt::BottomDockWidgetArea) ? 2
                          : (areaB == Qt::RightDockWidgetArea)  ? 3
                                                                : 4;
        if (areaOrderA != areaOrderB) {
            return areaOrderA < areaOrderB;
        }

        auto tabGroupA = tabifiedDockWidgets(a);
        auto tabGroupB = tabifiedDockWidgets(b);
        if (tabGroupA != tabGroupB) {
            auto ra = a->geometry();
            auto rb = b->geometry();
            if (ra.y() != rb.y()) {
                return ra.y() < rb.y(); // top-most first
            }
            return ra.x() < rb.x();
        }

        auto indexA = tabGroupA.indexOf(a);
        auto indexB = tabGroupA.indexOf(b);
        if (areaA == Qt::LeftDockWidgetArea) {
            return indexA > indexB;
        }
        if (areaA == Qt::RightDockWidgetArea) {
            return indexA < indexB;
        }
        if (areaA == Qt::TopDockWidgetArea) {
            return indexA < indexB;
        }
        if (areaA == Qt::BottomDockWidgetArea) {
            return indexA > indexB;
        }
        return indexA < indexB;
    });
    return docks;
}
