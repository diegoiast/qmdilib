/**
 * \file main3.cpp
 * \brief Configuration demo
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License GPL 2 or 3
 */

#include <QApplication>
#include <qmdiconfigdialog.h>
#include <qmdiglobalconfig.h>

auto getNetworkConfig() -> qmdiPluginConfig * {
    qmdiPluginConfig *networkPluginConfig = new qmdiPluginConfig();
    networkPluginConfig->pluginName = "NetworkPlugin";
    networkPluginConfig->description = "Configuration for network settings";
    networkPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                   .setKey("host")
                                                   .setType(qmdiConfigItem::String)
                                                   .setDisplayName("Host")
                                                   .setDescription("Network host address")
                                                   .setDefaultValue("localhost")
                                                   .build());
    networkPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                   .setKey("port")
                                                   .setType(qmdiConfigItem::UInt16)
                                                   .setDisplayName("Port")
                                                   .setDescription("Network port number")
                                                   .setDefaultValue(8080)
                                                   .build());
    networkPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                   .setKey("useSSL")
                                                   .setType(qmdiConfigItem::Bool)
                                                   .setDisplayName("Use ssl")
                                                   .setDescription("Use SSL for the connection?")
                                                   .setValue(true)
                                                   .setDefaultValue(true)
                                                   .build());
    networkPluginConfig->configItems.append(qmdiConfigItem::Builder()
                                                .setKey("dns")
                                                .setType(qmdiConfigItem::StringList)
                                                .setDisplayName("DNS deny list")
                                                .setDescription("Where not to connect")
                                                .setDefaultValue(QStringList() << "www.yahoo.com"
                                                                               << "cnn.com"
                                                                               << "apple.com")
                                                .build());

    return networkPluginConfig;
}

auto getEditorConfig(QFont defaultFont) -> qmdiPluginConfig * {
    qmdiPluginConfig *editorPluginConfig = new qmdiPluginConfig();
    editorPluginConfig->pluginName = "Editor";
    editorPluginConfig->description = "Configuration for text ediotr";
    editorPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                  .setKey("wraplines")
                                                  .setType(qmdiConfigItem::Bool)
                                                  .setDisplayName("Wrap long lines")
                                                  .setDefaultValue(true)
                                                  .build());
    editorPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                  .setKey("margin")
                                                  .setType(qmdiConfigItem::UInt16)
                                                  .setDisplayName("Margin line")
                                                  .setDefaultValue(80)
                                                  .build());
    editorPluginConfig->configItems.append(qmdiConfigItem::Builder()
                                               .setKey("lineendig")
                                               .setType(qmdiConfigItem::OneOf)
                                               .setDisplayName("Line ending style")
                                               .setValue(1)
                                               .setDefaultValue(2)
                                               .setPossibleValue(QStringList() << "Unix (cr)"
                                                                               << "Windows (cr+ln)"
                                                                               << "Keep original")
                                               .build());
    editorPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                  .setKey("font")
                                                  .setType(qmdiConfigItem::Font)
                                                  .setDisplayName("Display font")
                                                  .setDefaultValue(defaultFont)
                                                  .setValue(defaultFont)
                                                  .build());

    return editorPluginConfig;
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    auto globalConfig = qmdiGlobalConfig();
    auto networkPluginConfig = getNetworkConfig();
    auto editorPluginConfig = getEditorConfig(app.font());
    globalConfig.addPluginConfig(networkPluginConfig);
    globalConfig.addPluginConfig(editorPluginConfig);

    // API for accessing config:
    {
        // We can restore the defaults. When initializing the config
        // the values are set to defaults, so this is not strickly needed here.
        globalConfig.setDefaults();

        // Note how we can get the config from the plugin
        auto host = networkPluginConfig->getVariable<QString>("host");
        auto port = networkPluginConfig->getVariable<int>("port");

        // Or query the global config, for the plugin and the config item
        auto useSSL = globalConfig.getVariable<bool>("NetworkPlugin", "useSSL");
        auto denyList = globalConfig.getVariable<QStringList>("NetworkPlugin", "dns");

        // What happens if we use the wrong type?
        // Usually nothing, the proper conversion is done
        auto useSSLStr = globalConfig.getVariable<QString>("NetworkPlugin", "useSSL");
        auto useSSLInt = globalConfig.getVariable<int>("NetworkPlugin", "useSSL");

        qDebug() << "Config for network plugin";
        qDebug() << " host    = " << host;
        qDebug() << " port    = " << port;
        qDebug() << " useSSL  = " << useSSL;
        qDebug() << " list    = " << denyList;
        qDebug() << " useSSL (?) = " << useSSLStr;
        qDebug() << " useSSL (?) = " << useSSLInt;
    }

    // We can store the config on files. Only the values are saved.
    // We will load the user modifications
    globalConfig.loadFromFile("demo3-config.json");

    // We can ask users to modify th config. All the UI is auto generated
    // depending on the config defined by the plugins
    qmdiConfigDialog dialog(&globalConfig);
    if (dialog.exec()) {
        // ... and oviously saved into a file. Load the app again to reload the config
        globalConfig.saveToFile("demo3-config.json");
    }
}