#include <QtTest>
#include <qmdiglobalconfig.h>
#include <qmdiconfigwidgetfactory.h>

class CustomConfigTypeTests : public QObject {
    Q_OBJECT

  private slots:
    void testCustomConfigType();
};

void CustomConfigTypeTests::testCustomConfigType() {
    // 0. Register a handler for our custom type
    qmdiConfigWidgetRegistry::instance().registerCustomFactory("MyCustomType", []() {
        return std::make_unique<qmdiDefaultConfigWidgetFactory>();
    });

    // Create a plugin config with this custom type
    auto createConfig = []() -> qmdiPluginConfig * {
        qmdiPluginConfig *config = new qmdiPluginConfig();
        config->pluginName = "TestPlugin";
        config->description = "Test Plugin Description";
        config->configItems.push_back(qmdiConfigItem::Builder()
                                          .setKey("customColor")
                                          .setCustomType("MyCustomType")
                                          .setDisplayName("Custom Color")
                                          .setDefaultValue("#FFFFFF")
                                          .build());
        return config;
    };

    // 1. Create global config, add plugin, and set a non-default value
    QJsonObject json;
    {
        auto globalConfig = qmdiGlobalConfig();
        globalConfig.addPluginConfig(createConfig());

        // Set the value to something different from default
        globalConfig.setVariable("TestPlugin", "customColor", QString("#000000"));
        
        // Verify it's set in memory
        QCOMPARE(globalConfig.getVariable<QString>("TestPlugin", "customColor"), "#000000");

        // Save to JSON
        json = globalConfig.asJson();
    }

    // 2. Create a new global config, add plugin (defaults), and load from JSON
    {
        auto globalConfig = qmdiGlobalConfig();
        globalConfig.addPluginConfig(createConfig());

        // Verify default first
        QCOMPARE(globalConfig.getVariable<QString>("TestPlugin", "customColor"), "#FFFFFF");

        // Load JSON
        globalConfig.loadFromJson(json);

        // Verify the loaded value matches the saved value
        // This is expected to FAIL if the bug exists
        QCOMPARE(globalConfig.getVariable<QString>("TestPlugin", "customColor"), "#000000");
    }
}

QTEST_GUILESS_MAIN(CustomConfigTypeTests)
#include "customConfigTypeTests.moc"
