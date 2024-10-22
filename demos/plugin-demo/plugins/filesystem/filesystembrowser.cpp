/**
 * \file filesystembrowser.cpp
 * \brief Implementation of the configuration file system browser
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see ConfigDialog
 */

#include <QLineEdit>
#include <QListView>

#include "filesystembrowser.h"
#include "filesystemwidget.h"
#include "pluginmanager.h"
#include "qmdiserver.h"

FileSystemBrowserPlugin::~FileSystemBrowserPlugin() {}

FileSystemBrowserPlugin::FileSystemBrowserPlugin() {
    name = tr("File system browser");
    author = tr("Diego Iastrubni <diegoiast@gmail.com>");
    iVersion = 0;
    sVersion = "0.0.1";
    autoEnabled = true;
    alwaysEnabled = false;

    config.pluginName = "FileBrowserPlugin";
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey(Config::DisplayTreeKey)
                                     .setUserEditable(false)
                                     .setType(qmdiConfigItem::Bool)
                                     .build());
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey(Config::DirectoryKey)
                                     .setDefaultValue("")
                                     .setType(qmdiConfigItem::String)
                                     .setUserEditable(false)
                                     .build());
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey(Config::FilterKey)
                                     .setDefaultValue("")
                                     .setType(qmdiConfigItem::String)
                                     .setUserEditable(false)
                                     .build());
}

void FileSystemBrowserPlugin::on_client_merged(qmdiHost *host) {
    auto *pluginManager = dynamic_cast<PluginManager *>(host);
    panel = new FileSystemWidget;
    pluginManager->createNewPanel(Panels::West, tr("File system"), panel);

    connect(panel, &FileSystemWidget::fileDoubleClicked, [this](const QString &filePath) {
        PluginManager *pluginManager = dynamic_cast<PluginManager *>(mdiServer->mdiHost);
        if (pluginManager) {
            pluginManager->openFile(filePath);
        }
    });
}

void FileSystemBrowserPlugin::on_client_unmerged(qmdiHost *host) {
    // auto *pluginManager = dynamic_cast<PluginManager *>(host);
}

void FileSystemBrowserPlugin::loadConfig(QSettings &settings) {
    IPlugin::loadConfig(settings);
    auto savedDir = getConfig().getDirectory();
    auto savedFilter = getConfig().getFilter();
    auto isTreeVisible = getConfig().getDisplayTree();
    auto indexPath = this->panel->model->index(savedDir);

    this->panel->filterEdit->setText(savedFilter);
    this->panel->rootPathEdit->setText(QDir::toNativeSeparators(savedDir));
    this->panel->treeView->setRootIndex(indexPath);
    this->panel->iconView->setRootIndex(indexPath);
    if (isTreeVisible) {
        this->panel->showTreeView();
    } else {
        this->panel->showIconView();
    }
}

void FileSystemBrowserPlugin::saveConfig(QSettings &settings) {
    config.setVariable(Config::DisplayTreeKey, this->panel->isTreeVisible);
    config.setVariable(Config::DirectoryKey, this->panel->rootPathEdit->text());
    config.setVariable(Config::DisplayTreeKey, this->panel->isTreeVisible);
    IPlugin::saveConfig(settings);
}
