#ifndef __MAINWINDOW6_H__
#define __MAINWINDOW6_H__

#include <QMainWindow>
#include "qmdihost.h"

class QAction;
class QTabWidget;
class PluginManager;

class MainWindow6: public QMainWindow, public qmdiHost
{
	Q_OBJECT
public:
	MainWindow6( QWidget *owner = NULL);
	~MainWindow6();

public slots:
	void initGUI();
	void closeClient();
	void on_actionQuit_triggered();
	
private:
	QTabWidget	*tabWidget;
	QAction		*actionConfig;
	QAction		*actionQuit;

	PluginManager	*pluginManager;
};

#endif // __MAINWINDOW6_H__
