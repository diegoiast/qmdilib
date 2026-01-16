/**
 * \file main3.cpp
 * \brief Configuration demo
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License GPL 2 or 3
 */

#include <QApplication>
#include <QColorDialog>
#include <QPushButton>
#include <qmdiconfigwidgetfactory.h>
#include <qmdiconfigdialog.h>
#include <qmdiglobalconfig.h>

struct Point3D {
    int x = 0, y = 0, z = 0;
};
Q_DECLARE_METATYPE(Point3D)

class Point3DWidgetFactory : public qmdiTypedConfigWidgetFactory<Point3D> {
  public:
    static constexpr auto name = "Point3D";
    QWidget *createWidget(const qmdiConfigItem &item, const Point3D &initialValue,
                          qmdiConfigDialog *parent) override {
        Q_UNUSED(item);
        auto *lineEdit = new QLineEdit(parent);
        setValue(lineEdit, initialValue);
        return lineEdit;
    }

    Point3D value(QWidget *widget) override {
        auto *lineEdit = qobject_cast<QLineEdit *>(widget);
        if (!lineEdit) {
            return {};
        }

        QStringList parts = lineEdit->text().split(",");
        if (parts.size() != 3) {
            return {};
        }

        bool okX, okY, okZ;
        Point3D p;
        p.x = parts[0].trimmed().toInt(&okX);
        p.y = parts[1].trimmed().toInt(&okY);
        p.z = parts[2].trimmed().toInt(&okZ);

        if (!okX || !okY || !okZ) {
            return {};
        }
        return p;
    }

    void setValue(QWidget *widget, const Point3D &value) override {
        if (auto *lineEdit = qobject_cast<QLineEdit *>(widget)) {
            lineEdit->setText(QString("%1,%2,%3").arg(value.x).arg(value.y).arg(value.z));
        }
    }

    // Direct JSON handling
    Point3D parseValue(const qmdiConfigItem &item, const QJsonValue &v) override {
        Q_UNUSED(item);
        QStringList parts = v.toString().split(",");
        if (parts.size() != 3) {
            return {};
        }
        Point3D p;
        p.x = parts[0].toInt();
        p.y = parts[1].toInt();
        p.z = parts[2].toInt();
        return p;
    }

    QJsonValue serializeValue(const qmdiConfigItem &item, const Point3D &v) override {
        Q_UNUSED(item);
        return QString("%1,%2,%3").arg(v.x).arg(v.y).arg(v.z);
    }
};

class ColorWidgetFactory : public qmdiTypedConfigWidgetFactory<QColor> {
  public:
    static constexpr auto name = "Color";
    QWidget *createWidget(const qmdiConfigItem &item, const QColor &initialValue, qmdiConfigDialog *parent) override {
        QString colorName = initialValue.name();
        
        auto *btn = new QPushButton(parent);
        btn->setText(colorName);
        btn->setStyleSheet(QString("background-color: %1").arg(colorName));

        QObject::connect(btn, &QPushButton::clicked, [btn, parent]() {
            QColor initial(btn->text());
            QColor color = QColorDialog::getColor(initial, parent);
            if (color.isValid()) {
                btn->setText(color.name());
                btn->setStyleSheet(QString("background-color: %1").arg(color.name()));
            }
        });
        return btn;
    }

    QColor value(QWidget *widget) override {
        auto *btn = qobject_cast<QPushButton *>(widget);
        return btn ? QColor(btn->text()) : QColor{};
    }

    void setValue(QWidget *widget, const QColor &value) override {
        if (auto *btn = qobject_cast<QPushButton *>(widget)) {
            btn->setText(value.name());
            btn->setStyleSheet(QString("background-color: %1").arg(value.name()));
        }
    }

    QColor parseValue(const qmdiConfigItem &item, const QJsonValue &v) override {
        Q_UNUSED(item);
        return QColor(v.toString());
    }

    QJsonValue serializeValue(const qmdiConfigItem &item, const QColor &v) override {
        Q_UNUSED(item);
        return v.name(QColor::HexArgb);
    }
};

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
                                                  .setKey("backgroundColor")
                                                  .setCustomType(ColorWidgetFactory::name)
                                                  .setDisplayName("Background Color")
                                                  .setDefaultValue(QColor("#FFFFFF"))
                                                  .build());
    editorPluginConfig->configItems.push_back(qmdiConfigItem::Builder()
                                                  .setKey("position")
                                                  .setCustomType(Point3DWidgetFactory::name)
                                                  .setDisplayName("Position (x,y,z)")
                                                  .setDefaultValue(Point3D{10, 20, 30})
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

    // 1. Unified Registration for "Color": Handles UI, Parsing, and Serialization in one place.
    qmdiConfigWidgetRegistry::instance().registerCustomFactory<ColorWidgetFactory>(ColorWidgetFactory::name);
    qmdiConfigWidgetRegistry::instance().registerCustomFactory<Point3DWidgetFactory>(
        Point3DWidgetFactory::name);

    auto globalConfig = qmdiGlobalConfig();
    auto networkPluginConfig = getNetworkConfig();
    auto editorPluginConfig = getEditorConfig(app.font());
    globalConfig.addPluginConfig(networkPluginConfig);
    globalConfig.addPluginConfig(editorPluginConfig);

    // We can restore the defaults. When initializing the config
    // the values are set to defaults, so this is not strickly needed here.
    globalConfig.setDefaults();

    // We can store the config on files. Only the values are saved.
    // We will load the user modifications
    globalConfig.loadFromFile("demo3-config.json");

    // API for accessing config:
    {
        // Note how we can get the config from the plugin
        QString host = networkPluginConfig->getVariable<QString>("host");
        int port = networkPluginConfig->getVariable<int>("port");

        // Or query the global config, for the plugin and the config item
        bool useSSL = globalConfig.getVariable<bool>("NetworkPlugin", "useSSL");
        QStringList denyList = globalConfig.getVariable<QStringList>("NetworkPlugin", "dns");

        // What happens if we use the wrong type?
        // Usually nothing, the proper conversion is done
        QString useSSLStr = globalConfig.getVariable<QString>("NetworkPlugin", "useSSL");
        int useSSLInt = globalConfig.getVariable<int>("NetworkPlugin", "useSSL");
        
        // Note we can still use the QVariant system as wel.
        QVariant useSSLVariant = globalConfig.getVariable("NetworkPlugin", "useSSL");
        
        // Our custom widget.
        QColor color = globalConfig.getVariable<QColor>("Editor", "backgroundColor");
        QVariant colorVar = globalConfig.getVariable("Editor", "backgroundColor");
        
        // Another custom widget, with non Qt types
        // Point3DWidgetFactory pointFactory;
        Point3D point = editorPluginConfig->getVariable<Point3D>("position");

        qDebug() << "Config for network plugin";
        qDebug() << " host    = " << host;
        qDebug() << " port    = " << port;
        qDebug() << " useSSL  = " << useSSL;
        qDebug() << " list    = " << denyList;
        qDebug() << " useSSL (?) = " << useSSLStr;
        qDebug() << " useSSL (?) = " << useSSLInt;
        qDebug() << " useSSL (?) = " << useSSLVariant;
        qDebug() << "Config for editor";
        qDebug() << " color = " << color;
        qDebug() << " color (as variant) = " << colorVar;
        qDebug() << " position:" << point.x << point.y << point.z;
    }

    // We can ask users to modify the config. All the UI is auto generated
    // depending on the config defined by the plugins
    qmdiConfigDialog dialog(&globalConfig);
    if (dialog.exec()) {
        // ... and oviously saved into a file. Load the app again to reload the config
        globalConfig.saveToFile("demo3-config.json");
    }
}