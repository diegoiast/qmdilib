#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QIcon>
#include "ui_plugin_list.h"

class QIcon;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QListView;

class PluginManager;
class PluginModel;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog( QWidget *owner=NULL  );
	~ConfigDialog();
	void setManager( PluginManager *manager );

public slots:
	void applyChanges();

private:
	PluginManager	*pluginManager;
	PluginModel	*pluginModel;
	
	QListView	*contentsWidget;
	QStackedWidget	*pagesWidget;
	QPushButton	*applyButton;
	QPushButton	*closeButton;
	Ui::plugin_list plugin_list_ui;
// 	QLabel		*currentPageLabel;
};

#endif
