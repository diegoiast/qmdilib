#include <QtTest>
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
                                                   .setDefaultValue(true)
                                                   .setValue(true)
                                                   .build());

    return networkPluginConfig;
}

class TestQmdiGlobalConfig : public QObject {
    Q_OBJECT

  private slots:
    void testCodeConstruction();
    void testRestoreDefault();
    void testBasicSave();
    void testSaveLoad();
    void testLoadSchema();
    void testStringList();
};

void TestQmdiGlobalConfig::testCodeConstruction() {
    auto globalConfig = qmdiGlobalConfig();
    globalConfig.addPluginConfig(getNetworkConfig());
    auto config = globalConfig.getPluginConfig("NetworkPlugin");

    QVERIFY(config != nullptr);
    QCOMPARE(config->configItems.length(), 3);
    QCOMPARE(config->pluginName, "NetworkPlugin");
    QCOMPARE(config->description, "Configuration for network settings");
    QCOMPARE(config->getVariable<int>("port"), 8080);
    QCOMPARE(config->getVariable<QString>("host"), "localhost");
    QCOMPARE(config->getVariable<bool>("useSSL"), true);
    delete config;
}

void TestQmdiGlobalConfig::testRestoreDefault() {
    auto globalConfig = qmdiGlobalConfig();
    globalConfig.addPluginConfig(getNetworkConfig());

    globalConfig.setVariable("NetworkPlugin", "port", 111);
    globalConfig.setVariable("NetworkPlugin", "host", "");
    globalConfig.setVariable("NetworkPlugin", "useSSL", false);

    globalConfig.setDefaults();
    QCOMPARE(globalConfig.getVariable<bool>("NetworkPlugin", "useSSL"), true);
    QCOMPARE(globalConfig.getVariable<int>("NetworkPlugin", "port"), 8080);
    QCOMPARE(globalConfig.getVariable<QString>("NetworkPlugin", "host"), "localhost");
}

void TestQmdiGlobalConfig::testBasicSave() {
    auto globalConfig = qmdiGlobalConfig();
    globalConfig.addPluginConfig(getNetworkConfig());

    auto json = globalConfig.asJson();
    QVERIFY(!json.isEmpty());
    QVERIFY(json["NetworkPlugin"].isObject());
    QVERIFY(json["NetworkPlugin"].toObject()["configItems"].isArray());
    QCOMPARE(json["NetworkPlugin"].toObject()["configItems"].toArray().size(), 3);

    auto a = json["NetworkPlugin"].toObject()["configItems"].toArray()[0];
    QVERIFY(a.isObject());
    QVERIFY(a.toObject()["key"].toString() == "host");
    QVERIFY(a.toObject()["value"].toString() == "localhost");

    auto b = json["NetworkPlugin"].toObject()["configItems"].toArray()[1];
    QVERIFY(b.isObject());
    QVERIFY(b.toObject()["key"].toString() == "port");
    QVERIFY(b.toObject()["value"].toVariant().toInt() == 8080);

    auto c = json["NetworkPlugin"].toObject()["configItems"].toArray()[2];
    QVERIFY(c.isObject());
    QVERIFY(c.toObject()["key"].toString() == "useSSL");
    QVERIFY(b.toObject()["value"].toVariant().toBool() == true);
}

void TestQmdiGlobalConfig::testSaveLoad() {
    auto globalConfig = qmdiGlobalConfig();
    globalConfig.addPluginConfig(getNetworkConfig());

    auto json = globalConfig.asJson();
    globalConfig.loadFromJson(json);

    QCOMPARE(globalConfig.getVariable<bool>("NetworkPlugin", "useSSL"), true);
    QCOMPARE(globalConfig.getVariable<int>("NetworkPlugin", "port"), 8080);
    QCOMPARE(globalConfig.getVariable<QString>("NetworkPlugin", "host"), "localhost");

    globalConfig.setVariable("NetworkPlugin", "port", 111);
    globalConfig.setVariable("NetworkPlugin", "host", "");
    globalConfig.setVariable("NetworkPlugin", "useSSL", false);

    globalConfig.loadFromJson(json);
    QCOMPARE(globalConfig.getVariable<bool>("NetworkPlugin", "useSSL"), true);
    QCOMPARE(globalConfig.getVariable<int>("NetworkPlugin", "port"), 8080);
    QCOMPARE(globalConfig.getVariable<QString>("NetworkPlugin", "host"), "localhost");

    QString jsonString = R"(
        {
            "NetworkPlugin": {
                "configItems": [
                    {
                        "key": "host",
                        "value": "google.com"
                    },
                    {
                        "key": "port",
                        "value": "423"
                    },
                    {
                        "key": "useSSL",
                        "value": "true"
                    }
                ]
            }
        }
    )";
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    globalConfig.loadFromJson(jsonDoc.object());

    QCOMPARE(globalConfig.getVariable<int>("NetworkPlugin", "port"), 423);
    QCOMPARE(globalConfig.getVariable<QString>("NetworkPlugin", "host"), "google.com");
    QCOMPARE(globalConfig.getVariable<bool>("NetworkPlugin", "useSSL"), true);
}

void TestQmdiGlobalConfig::testLoadSchema() {
    QString jsonString = R"(
{
    "plugins": [
        {
            "pluginName": "NetworkPlugin",
            "description": "Handles network configurations",
            "configItems": [
                {
                    "key": "host",
                    "type": "String",
                    "displayName": "Host",
                    "description": "The network host",
                    "defaultValue": "localhost"
                },
                {
                    "key": "port",
                    "type": "Int32",
                    "displayName": "Port",
                    "description": "The network port",
                    "defaultValue": 8080
                },
                {
                    "key": "useSSL",
                    "type": "Bool",
                    "displayName": "Use SSL",
                    "description": "Enable SSL",
                    "defaultValue": true
                }
            ]
        },
        {
            "pluginName": "DatabasePlugin",
            "description": "Handles database configurations",
            "configItems": [
                {
                    "key": "dbHost",
                    "type": "String",
                    "displayName": "Database Host",
                    "description": "The database host",
                    "defaultValue": "localhost"
                },
                {
                    "key": "dbPort",
                    "type": "Int32",
                    "displayName": "Database Port",
                    "description": "The database port",
                    "defaultValue": 5432
                },
                {
                    "key": "useSSL",
                    "type": "Bool",
                    "displayName": "Use SSL",
                    "description": "Enable SSL for database",
                    "defaultValue": false
                }
            ]
        }
    ]
}
    )";
    auto jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    auto globalConfig = qmdiGlobalConfig();
    globalConfig.loadDefsFromJson(jsonDoc.object());
    QCOMPARE(globalConfig.plugins.size(), 2);

    QCOMPARE(globalConfig.plugins[0]->pluginName, "NetworkPlugin");
    QCOMPARE(globalConfig.plugins[0]->description, "Handles network configurations");
    QCOMPARE(globalConfig.plugins[0]->configItems.length(), 3);
    QCOMPARE(globalConfig.plugins[0]->configItems[0].displayName, "Host");
    QCOMPARE(globalConfig.plugins[0]->configItems[0].key, "host");
    QCOMPARE(globalConfig.plugins[0]->configItems[0].defaultValue, "localhost");
    QCOMPARE(globalConfig.getVariable<QString>("NetworkPlugin", "host"), "localhost");
    QCOMPARE(globalConfig.plugins[0]->configItems[1].displayName, "Port");
    QCOMPARE(globalConfig.plugins[0]->configItems[1].key, "port");
    QCOMPARE(globalConfig.plugins[0]->configItems[1].defaultValue, 8080);
    QCOMPARE(globalConfig.getVariable<int>("NetworkPlugin", "port"), 8080);
    QCOMPARE(globalConfig.plugins[0]->configItems[2].displayName, "Use SSL");
    QCOMPARE(globalConfig.plugins[0]->configItems[2].key, "useSSL");
    QCOMPARE(globalConfig.plugins[0]->configItems[2].defaultValue, true);
    QCOMPARE(globalConfig.getVariable<bool>("NetworkPlugin", "useSSL"), true);

    QCOMPARE(globalConfig.plugins[1]->pluginName, "DatabasePlugin");
    QCOMPARE(globalConfig.plugins[1]->description, "Handles database configurations");
    QCOMPARE(globalConfig.plugins[1]->configItems.length(), 3);
    QCOMPARE(globalConfig.plugins[1]->configItems[0].displayName, "Database Host");
    QCOMPARE(globalConfig.plugins[1]->configItems[0].key, "dbHost");
    QCOMPARE(globalConfig.plugins[1]->configItems[0].defaultValue, "localhost");
    QCOMPARE(globalConfig.getVariable<QString>("DatabasePlugin", "dbHost"), "localhost");
    QCOMPARE(globalConfig.plugins[1]->configItems[1].displayName, "Database Port");
    QCOMPARE(globalConfig.plugins[1]->configItems[1].key, "dbPort");
    QCOMPARE(globalConfig.plugins[1]->configItems[1].defaultValue, 5432);
    QCOMPARE(globalConfig.getVariable<int>("DatabasePlugin", "dbPort"), 5432);
    QCOMPARE(globalConfig.plugins[1]->configItems[2].displayName, "Use SSL");
    QCOMPARE(globalConfig.plugins[1]->configItems[2].key, "useSSL");
    QCOMPARE(globalConfig.plugins[1]->configItems[2].defaultValue, false);
    QCOMPARE(globalConfig.getVariable<bool>("DatabasePlugin", "useSSL"), false);
}

void TestQmdiGlobalConfig::testStringList() {
    auto listConfig = new qmdiPluginConfig();
    listConfig->pluginName = "list test";
    listConfig->configItems.push_back(qmdiConfigItem::Builder()
                                          .setKey("list")
                                          .setType(qmdiConfigItem::StringList)
                                          .setDisplayName("list")
                                          .setDescription("list")
                                          .setDefaultValue(QStringList() << "aaa")
                                          .setValue(QStringList() << "1"
                                                                  << "2")
                                          .build());
    auto globalConfig = qmdiGlobalConfig();
    globalConfig.addPluginConfig(listConfig);

    auto json = globalConfig.asJson();
    QVERIFY(!json.isEmpty());
    QVERIFY(json["list test"].isObject());
    QVERIFY(!json["list test"].toObject().isEmpty());
    QVERIFY(json["list test"].toObject()["configItems"].isArray());
    QCOMPARE(json["list test"].toObject()["configItems"].toArray().size(), 1);
    auto l = json["list test"].toObject()["configItems"].toArray()[0];
    QVERIFY(l.isObject());
    QCOMPARE(l.toObject()["key"].toString(), "list");
    QVERIFY(l.toObject()["value"].isArray());
    QCOMPARE(l.toObject()["value"].toArray()[0].toString(), "1");
    QCOMPARE(l.toObject()["value"].toArray()[1].toString(), "2");

    globalConfig.loadFromJson(json);
    auto ll = globalConfig.getVariable<QStringList>("list test", "list");
    QVERIFY(!ll.isEmpty());
}

QTEST_GUILESS_MAIN(TestQmdiGlobalConfig)
#include "globalConfigTest.moc"
