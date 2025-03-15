/**
 * \file main6.cpp
 * \brief Entry point of the 6st demo
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License GPL 2 or 3
 */

#include <QApplication>
#include <QMenu>
#include <QStatusBar>

#include "pluginmanager.h"
#include "plugins/editor/editor_plg.h"
#include "plugins/filesystem/filesystembrowser.h"
#include "plugins/help/help_plg.h"
#include "plugins/richtext/richtext_plg.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    PluginManager pluginManager;
    // pluginManager.setNativeSettingsManager( "Diego", "PluginManager" );
    pluginManager.setFileSettingsManager("plugin-demo.ini");

    // load a set of default plugins
    auto e = new EditorPlugin;
    pluginManager.addPlugin(new HelpPlugin);
    pluginManager.addPlugin(e);
    pluginManager.addPlugin(new RichTextPlugin);
    pluginManager.addPlugin(new FileSystemBrowserPlugin);
    pluginManager.updateGUI();
    pluginManager.hidePanels(Qt::BottomDockWidgetArea);

    // start the application
    pluginManager.restoreSettings();
    pluginManager.statusBar()->showMessage(
        QT_TR_NOOP("Welcome - feel free to configure the GUI to your needs"), 5000);
    pluginManager.show();

    pluginManager.connect(&pluginManager, &PluginManager::newFileRequested,
                          [e]() { e->fileNew(); });
    auto l = pluginManager.visibleTabs();
    if (l < 1) {
        e->fileNew();
    }
    return app.exec();
}
