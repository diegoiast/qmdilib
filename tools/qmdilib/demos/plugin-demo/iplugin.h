#ifndef __IPLUGIN_H__
#define __IPLUGIN_H__

#include <QObject>
#include <QString>
#include "qmdiclient.h"

class QWidget;
class QSettings;
class QAction;
class QIcon;

QAction* new_action( QIcon icon, QString name, QObject *obj, QString shortcut, QString status_tip, const char *slot );

class IPlugin: public QObject, public qmdiClient
{
friend class PluginManager;

public:
	IPlugin();
	virtual ~IPlugin();

	virtual void showAbout();
	virtual QWidget* getConfigDialog();
	virtual void getData();
	virtual void setData();

	virtual void loadConfig( QSettings &settings );
	virtual void SaveConfig( QSettings &settings );

	bool 	isEnabled();
	QString	getName();
	QString getAuthor();
	QString getsVersion();
	int	getiVersion();
	
protected:
	QString name;
	QString author;
	QString sVersion;
	int	iVersion;
	
	bool alwaysEnabled;
	bool autoEnabled;
	bool enabled;
};
#endif // __IPLUGIN_H__
