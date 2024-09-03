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

QTEST_GUILESS_MAIN(TestQmdiGlobalConfig)
#include "globalConfigTest.moc"
