/**
 * \file help_plg.cpp
 * \brief Implementation of the HelpPlugin class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL
 * \see HelpPlugin
 */

// $Id$

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QLibraryInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QProcess>
#include <QStatusBar>
#include <QUrl>

#include "help_plg.h"
#include "helpbrowse.h"
#include "iplugin.h"
#include "qmdiserver.h"

HelpPlugin::HelpPlugin() : IPlugin() {
    name = tr("Help plugin");
    author = "Diego Iastrubni <diegoiast@gmail.com>";
    iVersion = 0;
    sVersion = "0.0.1";
    autoEnabled = true;
    alwaysEnabled = true;

    actionAbout =
        new_action(QIcon(), tr("&About"), this, QString(), tr("XXXXX"), SLOT(showAboutApp()));
    actionAboutQt =
        new_action(QIcon(), tr("&About Qt"), this, QString(), tr("XXXXX"), SLOT(showAboutQt()));
    actionShowQtHelp =
        new_action(QIcon(), tr("&Qt help"), this, "Ctrl+F1", tr("XXXXX"), SLOT(showQtHelp()));

    menus["&Help"]->addAction(actionShowQtHelp);
    menus["&Help"]->setMergePoint();
    menus["&Help"]->addSeparator();
    menus["&Help"]->addAction(actionAbout);
    menus["&Help"]->addAction(actionAboutQt);

    config.pluginName = "HelpPlugin";
    config.description = tr("Configuration for the help system");
    // 	externalBrowser = "/opt/kde3/bin/konqueror";

    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey("externalBrowser")
                                     .setType(qmdiConfigItem::String)
                                     .setDisplayName(tr("External internet browser"))
                                     .setDescription(tr("Where is firefox is intalled?"))
                                     .setDefaultValue(QString{})
                                     .build());

    // As an example, we define a random configuration for network. Not
    // used by this application at all.
    // config.pluginName = "NetworkPlugin";
    // config.description = tr("Configuration for network settings");
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey("host")
                                     .setType(qmdiConfigItem::String)
                                     .setDisplayName(tr("Host"))
                                     .setDescription(tr("Network host address"))
                                     .setDefaultValue("localhost")
                                     .build());
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey("port")
                                     .setType(qmdiConfigItem::UInt16)
                                     .setDisplayName(tr("Port"))
                                     .setDescription(tr("Network port number"))
                                     .setDefaultValue(8080)
                                     .build());
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey("useSSL")
                                     .setType(qmdiConfigItem::Bool)
                                     .setDisplayName(tr("Use ssl"))
                                     .setDescription(tr("Use SSL for the connection?"))
                                     .setValue(true)
                                     .setDefaultValue(true)
                                     .build());
    config.configItems.append(qmdiConfigItem::Builder()
                                  .setKey("dns")
                                  .setType(qmdiConfigItem::StringList)
                                  .setDisplayName(tr("DNS deny list"))
                                  .setDescription(tr("Where not to connect"))
                                  .setDefaultValue(QStringList() << "www.yahoo.com"
                                                                 << "cnn.com"
                                                                 << "apple.com")
                                  .build());
}

HelpPlugin::~HelpPlugin() {
    delete actionAbout;
    delete actionAboutQt;
    delete actionShowQtHelp;
}

void HelpPlugin::showAbout() {}

void HelpPlugin::showAboutApp() {
    QMessageBox::about(0, "qmdilib demo",
                       "A small demo for loading plugins<br>Diego Iastrubni "
                       "(diegoiast@gmail.com) - lgpl)");
}

void HelpPlugin::showAboutQt() { QApplication::aboutQt(); }

void HelpPlugin::showQtHelp() {
    QString helpFile =
        QLibraryInfo::path(QLibraryInfo::DocumentationPath) + QLatin1String("/html/index.html");
    loadHTML(helpFile);
}

void HelpPlugin::on_browser_sourceChanged(const QUrl &src) {
    if (externalBrowser.isEmpty()) {
        return;
    }

    if ((src.scheme() == "file") || (src.scheme().isEmpty())) {
        return;
    }

    if (!QProcess::startDetached(externalBrowser, QStringList(src.toString()))) {
        QWidget *w = dynamic_cast<QWidget *>(mdiServer);
        if (w == NULL) {
            return;
        }

        QMainWindow *ww = dynamic_cast<QMainWindow *>(w->window());
        if (ww) {
            ww->statusBar()->showMessage("Error: could not start external browser", 5000);
        }
    }
}

int HelpPlugin::canOpenFile(const QString fileName) {
    QUrl u(fileName);

    if (u.scheme().toLower() != "help") {
        return -1;
    }

    // now, lets assume this is a class name or a full qualified file name
    QString className = u.path().toLower();
    if ((QFile::exists(QLibraryInfo::path(QLibraryInfo::DocumentationPath) +
                       QLatin1String("/html/") + className + QLatin1String(".html"))) ||
        QFile::exists(className)) {
        return 10;
    } else {
        return -1;
    }
}

bool HelpPlugin::openFile(const QString fileName, int x, int y, int z) {
    QUrl u(fileName);
    bool fileLoaded = false;

    if (QFile::exists(QLibraryInfo::path(QLibraryInfo::DocumentationPath) +
                      QLatin1String("/html/") + fileName + QLatin1String(".html"))) {
        fileLoaded = canOpenFile(fileName) > 0;
        if (fileLoaded) {
            fileLoaded =
                loadHTML(QLibraryInfo::path(QLibraryInfo::DocumentationPath) +
                             QLatin1String("/html/") + u.path().toLower() + QLatin1String(".html"),
                         x, y, z);
        }
    } else if (QFile::exists(u.path())) {
        fileLoaded = loadHTML(u.path(), x, y, z);
    }

    return fileLoaded;
}

bool HelpPlugin::loadHTML(QString fileName, int x, int y, int z) {
    QexHelpBrowser *browser = new QexHelpBrowser(QUrl("file:" + fileName), true);
    browser->mdiClientName = tr("Qt help");
    browser->setObjectName(browser->mdiClientName);
    connect(browser, SIGNAL(sourceChanged(QUrl)), this, SLOT(on_browser_sourceChanged(QUrl)));

    mdiServer->addClient(browser);

    return true;
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(z);
}
