#include <QtTest>
#include <qmdipluginconfig.h>

class TestQmdiPluginConfig : public QObject {
    Q_OBJECT

  private slots:
    void testDefaultConstruction();
    void testSetAndGetVariable();
    void testGetVariableWithDefault();
    void testRestoreDefault();
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

QTEST_MAIN(TestQmdiPluginConfig)
#include "pluginConfigTests.moc"
