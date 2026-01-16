#include <QtTest>
#include <qmdiglobalconfig.h>
#include <qmdiconfigwidgetfactory.h>
#include <QColor>

// A minimal factory for tests that only need data conversion, no UI.
template <typename T>
class SimpleTypeFactory : public qmdiTypedConfigWidgetFactory<T> {
public:
    using Parser = std::function<T(const QJsonValue&)>;
    using Serializer = std::function<QJsonValue(const T&)>;

    SimpleTypeFactory(Parser p, Serializer s = nullptr) : m_parser(p), m_serializer(s) {}

    // Data
    T parseValue(const qmdiConfigItem& item, const QJsonValue& v) override { 
        Q_UNUSED(item);
        return m_parser(v); 
    }
    QJsonValue serializeValue(const qmdiConfigItem& item, const T& v) override { 
        Q_UNUSED(item);
        return m_serializer ? m_serializer(v) : qmdiTypedConfigWidgetFactory<T>::serializeValue(item, v); 
    }

    // UI (Not used in these tests)
    QWidget* createWidget(const qmdiConfigItem&, const T&, qmdiConfigDialog*) override { return nullptr; }
    T value(QWidget*) override { return T(); }
    void setValue(QWidget*, const T&) override {}

private:
    Parser m_parser;
    Serializer m_serializer;
};

class RegistrationTests : public QObject {
    Q_OBJECT

  private slots:
    void cleanup() {
        qmdiConfigWidgetRegistry::instance().clearCustomFactories();
    }
    void testCustomTypeRegistration();
    void testMultipleCustomTypes();
    void testInvalidCustomTypes();
    void testCustomTypeRoundTrip();
};

void RegistrationTests::testCustomTypeRegistration() {
    // 1. Register a custom "Color" type via unified registry
    qmdiConfigWidgetRegistry::instance().registerCustomFactory("Color", []() {
        return std::make_unique<SimpleTypeFactory<QColor>>(
            [](const QJsonValue &val) { return QColor(val.toString()); },
            [](const QColor &c) { return c.name(QColor::HexArgb); }
        );
    });


    // 2. Define a JSON configuration using the custom type
    QJsonObject colorItem;
    colorItem["key"] = "bg_color";
    colorItem["type"] = "Color";
    colorItem["displayName"] = "Background Color";
    colorItem["description"] = "The color of the background";
    colorItem["defaultValue"] = "#FF0000";

    QJsonArray configItems;
    configItems.append(colorItem);

    QJsonObject plugin;
    plugin["pluginName"] = "VisualPlugin";
    plugin["description"] = "A plugin for visuals";
    plugin["configItems"] = configItems;

    QJsonArray plugins;
    plugins.append(plugin);

    QJsonObject root;
    root["plugins"] = plugins;

    // 3. Load the definition
    qmdiGlobalConfig config;
    bool success = config.loadDefsFromJson(root);

    // 4. Verify
    QVERIFY(success);
    qmdiPluginConfig *pConfig = config.getPluginConfig("VisualPlugin");
    QVERIFY(pConfig != nullptr);
    QCOMPARE(pConfig->configItems.size(), 1);

    const qmdiConfigItem &item = pConfig->configItems[0];
    QCOMPARE(item.type, qmdiConfigItem::Custom);
    QCOMPARE(item.customTypeString, QString("Color"));
    
    // Check if the parser was actually called and returned a QColor
    QVariant val = item.defaultValue;
    QCOMPARE(val.userType(), qMetaTypeId<QColor>());
    QCOMPARE(val.value<QColor>(), QColor("#FF0000"));
}

void RegistrationTests::testMultipleCustomTypes() {
    // Register "Color" as well
    qmdiConfigWidgetRegistry::instance().registerCustomFactory("Color", []() {
        return std::make_unique<SimpleTypeFactory<QColor>>(
            [](const QJsonValue &val) { return QColor(val.toString()); },
            [](const QColor &c) { return c.name(QColor::HexArgb); }
        );
    });

    // 1. Register "Point" type via unified registry
    qmdiConfigWidgetRegistry::instance().registerCustomFactory("Point", []() {
        return std::make_unique<SimpleTypeFactory<QPoint>>([](const QJsonValue &val) {
            // format "x,y"
            QStringList parts = val.toString().split(",");
            if (parts.size() == 2) {
                return QPoint(parts[0].toInt(), parts[1].toInt());
            }
            return QPoint();
        });
    });

    // 2. Define JSON with BOTH Color and Point
    QJsonObject colorItem;
    colorItem["key"] = "bg_color";
    colorItem["type"] = "Color";
    colorItem["defaultValue"] = "#00FF00";

    QJsonObject pointItem;
    pointItem["key"] = "start_pos";
    pointItem["type"] = "Point";
    pointItem["defaultValue"] = "10,20";

    QJsonArray configItems;
    configItems.append(colorItem);
    configItems.append(pointItem);

    QJsonObject plugin;
    plugin["pluginName"] = "GeometryPlugin";
    plugin["configItems"] = configItems;

    QJsonObject root;
    root["plugins"] = QJsonArray{plugin};

    // 3. Load
    qmdiGlobalConfig config;
    bool success = config.loadDefsFromJson(root);

    // 4. Verify
    QVERIFY(success);
    auto pConfig = config.getPluginConfig("GeometryPlugin");
    QVERIFY(pConfig != nullptr);
    QCOMPARE(pConfig->configItems.size(), 2);

    // Verify Color
    auto colorConf = pConfig->configItems[0];
    QCOMPARE(colorConf.key, QString("bg_color"));
    QCOMPARE(colorConf.type, qmdiConfigItem::Custom);
    QCOMPARE(colorConf.customTypeString, QString("Color"));
    QCOMPARE(colorConf.defaultValue.value<QColor>(), QColor("#00FF00"));

    // Verify Point
    auto pointConf = pConfig->configItems[1];
    QCOMPARE(pointConf.key, QString("start_pos"));
    QCOMPARE(pointConf.type, qmdiConfigItem::Custom);
    QCOMPARE(pointConf.customTypeString, QString("Point"));
    QCOMPARE(pointConf.defaultValue.value<QPoint>(), QPoint(10, 20));
}

void RegistrationTests::testInvalidCustomTypes() {
    // Register handlers
    qmdiConfigWidgetRegistry::instance().registerCustomFactory("Color", []() {
        return std::make_unique<SimpleTypeFactory<QColor>>(
            [](const QJsonValue &val) { return QColor(val.toString()); },
            [](const QColor &c) { return c.name(QColor::HexArgb); }
        );
    });
    qmdiConfigWidgetRegistry::instance().registerCustomFactory("Point", []() {
        return std::make_unique<SimpleTypeFactory<QPoint>>([](const QJsonValue &val) {
            QStringList parts = val.toString().split(",");
            if (parts.size() == 2) {
                return QPoint(parts[0].toInt(), parts[1].toInt());
            }
            return QPoint();
        });
    });

    // 1. Define JSON with invalid data for Color and Point
    QJsonObject colorItem;
    colorItem["key"] = "bad_color";
    colorItem["type"] = "Color";
    colorItem["defaultValue"] = "#20,af"; // Invalid color

    QJsonObject pointItem;
    pointItem["key"] = "bad_point";
    pointItem["type"] = "Point";
    pointItem["defaultValue"] = "#20,af"; // Invalid integers

    QJsonArray configItems;
    configItems.append(colorItem);
    configItems.append(pointItem);

    QJsonObject plugin;
    plugin["pluginName"] = "ErrorPlugin";
    plugin["configItems"] = configItems;

    QJsonObject root;
    root["plugins"] = QJsonArray{plugin};

    // 2. Load
    qmdiGlobalConfig config;
    bool success = config.loadDefsFromJson(root);
    QVERIFY(success);

    auto pConfig = config.getPluginConfig("ErrorPlugin");
    
    // 3. Verify Color behavior
    auto colorConf = pConfig->configItems[0];
    QVariant colorVal = colorConf.defaultValue;
    // It is a QColor type...
    QCOMPARE(colorVal.userType(), qMetaTypeId<QColor>());
    // ...but it is "invalid" (alpha is invalid, rgb is 0)
    QVERIFY(!colorVal.value<QColor>().isValid());

    // 4. Verify Point behavior
    auto pointConf = pConfig->configItems[1];
    QVariant pointVal = pointConf.defaultValue;
    // toInt() returns 0 on failure, so we get (0,0)
    QCOMPARE(pointVal.value<QPoint>(), QPoint(0, 0));
}

void RegistrationTests::testCustomTypeRoundTrip() {
    // 1. Register "Size" type via unified registry
    qmdiConfigWidgetRegistry::instance().registerCustomFactory("Size", []() {
        return std::make_unique<SimpleTypeFactory<QSize>>(
            // Parser: JSON Array [w, h] -> QSize
            [](const QJsonValue &val) {
                QJsonArray arr = val.toArray();
                return QSize(arr[0].toInt(), arr[1].toInt());
            },
            // Serializer: QSize -> JSON Array [w, h]
            [](const QSize &s) {
                return QJsonArray{s.width(), s.height()};
            }
        );
    });

    // 2. Define plugin with default value
    QJsonObject sizeItem;
    sizeItem["key"] = "window_size";
    sizeItem["type"] = "Size";
    sizeItem["defaultValue"] = QJsonArray{800, 600};

    QJsonObject plugin;
    plugin["pluginName"] = "WindowPlugin";
    plugin["configItems"] = QJsonArray{sizeItem};
    QJsonObject root;
    root["plugins"] = QJsonArray{plugin};

    qmdiGlobalConfig config;
    config.loadDefsFromJson(root);

    // 3. Modify the value
    config.setVariable("WindowPlugin", "window_size", QSize(1024, 768));

    // 4. Serialize to JSON (Simulate Save)
    QJsonObject savedJson = config.asJson();
    
    // Verify the saved JSON structure
    QJsonObject savedPlugin = savedJson["WindowPlugin"].toObject();
    QJsonArray savedItems = savedPlugin["configItems"].toArray();
    QJsonObject savedItem = savedItems[0].toObject(); // Find by iteration in real code, but here it's 0
    QJsonArray savedValue = savedItem["value"].toArray();
    
    QCOMPARE(savedValue[0].toInt(), 1024);
    QCOMPARE(savedValue[1].toInt(), 768);

    // 5. Deserialize from JSON (Simulate Load User Config)
    qmdiGlobalConfig config2;
    config2.loadDefsFromJson(root); // Load defaults first
    config2.fromJson(savedJson);    // Apply overrides

    // Verify the value was restored correctly using the Parser
    QSize restoredSize = config2.getVariable<QSize>("WindowPlugin", "window_size");
    QCOMPARE(restoredSize, QSize(1024, 768));
}

QTEST_GUILESS_MAIN(RegistrationTests)
#include "registrationTests.moc"
