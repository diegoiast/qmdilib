/**
 * \file filesystembrowser.h
 * \brief Declaration of the configuration file system browser
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see ConfigDialog
 */

#pragma once

#include "iplugin.h"

class FileSystemWidget;

class FileSystemBrowserPlugin : public IPlugin {
  public:
    struct Config {
        CONFIG_DEFINE(DisplayTree, bool)
        CONFIG_DEFINE(Filter, QString)
        CONFIG_DEFINE(Directory, QString)
        qmdiPluginConfig *config;
    };
    Config &getConfig() {
        static Config configObject{&this->config};
        return configObject;
    }

    FileSystemBrowserPlugin();
    ~FileSystemBrowserPlugin();
    virtual void on_client_merged(qmdiHost *host) override;
    virtual void loadConfig(QSettings &settings) override;
    virtual void saveConfig(QSettings &settings) override;

  private:
    FileSystemWidget *panel = nullptr;
};
