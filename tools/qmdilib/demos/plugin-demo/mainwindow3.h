#ifndef __MAINWINDOW3_H__
#define __MAINWINDOW3_H__

#include <QMainWindow>
#include "qmdihost.h"

class QAction;
class QTabWidget;
class PluginManager;

class MainWindow3: public QMainWindow, public qmdiHost
{
	Q_OBJECT
public:
	MainWindow3( QWidget *owner = NULL);
	~MainWindow3();

public slots:
	void initGUI();
	
private:
	QTabWidget	*tabWidget;
	QAction		*actionConfig;

	PluginManager	*pluginManager;
};

#endif // __MAINWINDOW3_H__
