#include <QtTest>
#include <qmdipluginconfig.h>

class TestQmdiPluginConfig : public QObject {
    Q_OBJECT

  private slots:
    void testDefaultConstruction();
    void testSetAndGetVariable();
    void testGetVariableWithDefault();
    void testRestoreDefault();
    void testModifyAndDefault();
    void testStringList();
};

void TestQmdiPluginConfig::testDefaultConstruction() {
    qmdiPluginConfig config;
    QCOMPARE(config.pluginName, QString());
    QCOMPARE(config.description, QString());
    QCOMPARE(config.configItems.size(), 0);
}

void TestQmdiPluginConfig::testSetAndGetVariable() {
    qmdiPluginConfig config;
    config.configItems.append(qmdiConfigItem::Builder()
                                  .setKey("exampleKey")
                                  .setType(qmdiConfigItem::String)
                                  .setDefaultValue("defaultValue")
                                  .setValue("actualValue")
                                  .build());

    QString result = config.getVariable<QString>("exampleKey");
    QCOMPARE(result, "actualValue");
}

void TestQmdiPluginConfig::testGetVariableWithDefault() {
    qmdiPluginConfig config;
    config.configItems.append(qmdiConfigItem::Builder()
                                  .setKey("exampleKey1")
                                  .setType(qmdiConfigItem::String)
                                  .setDefaultValue("defaultValue1")
                                  .setValue("actualValue1")
                                  .build());
    config.configItems.append(qmdiConfigItem::Builder()
                                  .setKey("exampleKey2")
                                  .setType(qmdiConfigItem::String)
                                  .setDefaultValue("defaultValue2")
                                  .build());

    QString result = config.getVariable<QString>("exampleKey1");
    QCOMPARE(result, "actualValue1");

    config.setVariable<QString>("exampleKey1", "other string");
    result = config.getVariable<QString>("exampleKey1");
    QCOMPARE(result, "other string");

    result = config.getVariable<QString>("exampleKey2");
    QCOMPARE(result, "defaultValue2");
}

void TestQmdiPluginConfig::testRestoreDefault() {

    qmdiPluginConfig config;
    config.configItems.append(qmdiConfigItem::Builder()
                                  .setKey("exampleKey1")
                                  .setType(qmdiConfigItem::String)
                                  .setDefaultValue("defaultValue1")
                                  .setValue("actualValue1")
                                  .build());
    config.configItems.append(qmdiConfigItem::Builder()
                                  .setKey("exampleKey2")
                                  .setType(qmdiConfigItem::String)
                                  .setDefaultValue("defaultValue2")
                                  .build());

    config.setVariable<QString>("exampleKey1", "other string");
    config.setDefault();

    QString result;
    result = config.getVariable<QString>("exampleKey1");
    QCOMPARE(result, "defaultValue1");
    result = config.getVariable<QString>("exampleKey2");
    QCOMPARE(result, "defaultValue2");
}

void TestQmdiPluginConfig::testModifyAndDefault() {
    qmdiPluginConfig *networkPluginConfig = new qmdiPluginConfig();
    networkPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                   .setKey("useSSL")
                                                   .setType(qmdiConfigItem::Bool)
                                                   .setDisplayName("Use ssl")
                                                   .setDescription("Use SSL for the connection?")
                                                   .setDefaultValue(true)
                                                   .setValue(true)
                                                   .build());
    QCOMPARE(networkPluginConfig->getVariable<bool>("useSSL"), true);

    networkPluginConfig->setVariable("useSSL", false);
    QCOMPARE(networkPluginConfig->getVariable<bool>("useSSL"), false);

    networkPluginConfig->setDefault();
    QCOMPARE(networkPluginConfig->getVariable<bool>("useSSL"), true);
}

void TestQmdiPluginConfig::testStringList() {
    qmdiPluginConfig *networkPluginConfig = new qmdiPluginConfig();
    networkPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                   .setKey("list")
                                                   .setType(qmdiConfigItem::StringList)
                                                   .setDisplayName("list")
                                                   .setDescription("list")
                                                   .setDefaultValue(QStringList() << "aaa")
                                                   .setValue(QStringList() << "1"
                                                                           << "2")
                                                   .build());

    auto l = networkPluginConfig->getVariable<QStringList>("list");
    QCOMPARE(l.size(), 2);
    QCOMPARE(l[0], "1");
    QCOMPARE(l[1], "2");

    networkPluginConfig->setDefault();
    l = networkPluginConfig->getVariable<QStringList>("list");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l[0], "aaa");
}

QTEST_MAIN(TestQmdiPluginConfig)
#include "pluginConfigTests.moc"
