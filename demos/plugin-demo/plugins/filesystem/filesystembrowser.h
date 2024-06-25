/**
 * \file filesystembrowser.h
 * \brief Declaration of the configuration file system browser
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see ConfigDialog
 */

#pragma once

#include "iplugin.h"

class QFileSystemModel;
class QTreeView;
    
class FileSystemBrowserPlugin : public IPlugin {
  public:
    FileSystemBrowserPlugin();
    ~FileSystemBrowserPlugin();
    void on_client_merged(qmdiHost *host);
    void on_client_unmerged(qmdiHost *host);
};
