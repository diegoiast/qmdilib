#include <qmdipluginconfig.h>

#include <QtTest>

class TestQmdiConfigItem : public QObject {
    Q_OBJECT

  private slots:
    void typeFromString();
    void typeToString();
    void builderSetters();
    void builderStringList();
    void configDefaults();
};

void TestQmdiConfigItem::typeFromString() {
    QCOMPARE(qmdiConfigItem::typeFromString("String"), qmdiConfigItem::String);
    QCOMPARE(qmdiConfigItem::typeFromString("Bool"), qmdiConfigItem::Bool);
    QCOMPARE(qmdiConfigItem::typeFromString("Int8"), qmdiConfigItem::Int8);
    QCOMPARE(qmdiConfigItem::typeFromString("Int16"), qmdiConfigItem::Int16);
    QCOMPARE(qmdiConfigItem::typeFromString("Int32"), qmdiConfigItem::Int32);
    QCOMPARE(qmdiConfigItem::typeFromString("UInt8"), qmdiConfigItem::UInt8);
    QCOMPARE(qmdiConfigItem::typeFromString("UInt16"), qmdiConfigItem::UInt16);
    QCOMPARE(qmdiConfigItem::typeFromString("UInt32"), qmdiConfigItem::UInt32);
    QCOMPARE(qmdiConfigItem::typeFromString("Float"), qmdiConfigItem::Float);
    QCOMPARE(qmdiConfigItem::typeFromString("Double"), qmdiConfigItem::Double);
    QCOMPARE(qmdiConfigItem::typeFromString("StringList"), qmdiConfigItem::StringList);

    QCOMPARE(qmdiConfigItem::typeFromString("InvalidType"),
             qmdiConfigItem::String); // Assuming default is String
}

void TestQmdiConfigItem::typeToString() {
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::String), "String");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::Bool), "Bool");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::Int8), "Int8");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::Int16), "Int16");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::Int32), "Int32");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::UInt8), "UInt8");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::UInt16), "UInt16");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::UInt32), "UInt32");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::Float), "Float");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::Double), "Double");
    QCOMPARE(qmdiConfigItem::typeToString(qmdiConfigItem::StringList), "StringList");

    QCOMPARE(qmdiConfigItem::typeToString(static_cast<qmdiConfigItem::ClassType>(999)),
             "Unknown"); // Assuming default for unknown types
}

void TestQmdiConfigItem::builderSetters() {
    qmdiConfigItem::Builder builder;
    builder.setKey("exampleKey")
        .setType(qmdiConfigItem::Int32)
        .setDisplayName("Example Display Name")
        .setDescription("Example Description")
        .setDefaultValue(42)
        .setValue(84);

    qmdiConfigItem item = builder.build();

    QCOMPARE(item.key, "exampleKey");
    QCOMPARE(item.type, qmdiConfigItem::Int32);
    QCOMPARE(item.displayName, "Example Display Name");
    QCOMPARE(item.description, "Example Description");
    QCOMPARE(item.defaultValue.toInt(), 42);
    QCOMPARE(item.value.toInt(), 84);
}

void TestQmdiConfigItem::builderStringList() {
    qmdiConfigItem item = qmdiConfigItem::Builder()
                              .setKey("dns")
                              .setType(qmdiConfigItem::StringList)
                              .setDisplayName("DNS deny list")
                              .setDescription("Where not to connect")
                              .setValue(QStringList() << "www.yahoo.com"
                                                      << "cnn.com"
                                                      << "apple.com")
                              .setDefaultValue(QStringList())
                              .build();
    auto l = item.value.toStringList();
    QCOMPARE(l.size(), 3);
}

void TestQmdiConfigItem::configDefaults() {
    qmdiConfigItem::Builder builder;
    builder.setKey("exampleKey")
        .setType(qmdiConfigItem::Int32)
        .setDisplayName("Example Display Name")
        .setDescription("Example Description")
        .setDefaultValue(42)
        .setValue(84);

    qmdiConfigItem item = builder.build();
    QCOMPARE_NE(item.value.toInt(), 42);
    QCOMPARE(item.value.toInt(), 84);

    item.setDefault();
    QCOMPARE(item.key, "exampleKey");
    QCOMPARE(item.type, qmdiConfigItem::Int32);
    QCOMPARE(item.displayName, "Example Display Name");
    QCOMPARE(item.description, "Example Description");
    QCOMPARE(item.defaultValue.toInt(), 42);
    QCOMPARE(item.value.toInt(), 42);
}

QTEST_MAIN(TestQmdiConfigItem)
#include "configItemTests.moc"
