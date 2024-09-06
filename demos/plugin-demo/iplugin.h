#pragma once

/**
 * \file iplugin.h
 * \brief Declaration of the IPlugin interface
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see IPlugin
 */

#include "pluginmanager.h"
#include "qmdiclient.h"
#include "qmdipluginconfig.h"
#include "qmdiserver.h"
#include <QObject>

class QString;
class QWidget;
class QSettings;
class QAction;
class QActionGroup;
class QIcon;

// Used to help building config
// clang-format off
#define CONFIG_DEFINE(key, type) \
    static constexpr auto key##Key = #key; \
    type get##key() const { \
        return config->getVariable<type>(key##Key); \
    } \
    void set##key(const type &value) { \
        config->setVariable<type>(key##Key, value); \
    }
// clang-format on

QAction *new_action(QIcon icon, QString name, QObject *obj, QString shortcut, QString status_tip,
                    const char *slot);

class IPlugin : public QObject, public qmdiClient {
    friend class PluginManager;

  public:
    IPlugin();
    virtual ~IPlugin();

    virtual void showAbout();
    virtual QWidget *getConfigDialog();
    virtual void getData();
    virtual void setData();

    virtual void loadConfig(QSettings &settings);
    virtual void saveConfig(QSettings &settings);

    virtual QActionGroup *newFileActions();
    virtual QStringList myExtensions();
    virtual int canOpenFile(const QString fileName);
    virtual bool openFile(const QString fileName, int x = -1, int y = -1, int z = -1);
    virtual void navigateFile(qmdiClient *client, int x, int y, int z);

    bool isEnabled();
    void setEnabled(bool enable);
    bool canDisable();
    QIcon getIcon();
    QString getName();
    QString getAuthor();
    QString getsVersion();
    int getiVersion();

    PluginManager *getManager() const {
        if (!mdiServer || !mdiServer->mdiHost) {
            return nullptr;
        }
        return dynamic_cast<PluginManager *>(mdiServer->mdiHost);
    }

    qmdiPluginConfig config;

  public slots:
    virtual void configurationHasBeenModified() {}

  protected:
    QString name;
    QString author;
    QString sVersion;
    int iVersion;

    bool alwaysEnabled;
    bool autoEnabled;
    bool enabled;
};
