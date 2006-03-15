#ifndef __MAIN_WIN_H__
#define __MAIN_WIN_H__

/**
 * \file mainwindow.h
 * \brief Definition of the main window class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

#include <QMainWindow>
#include "qmdihost.h"
#include "qmdiclient.h"
#include "qmditabwidget.h"


class QTabWidget;

class MainWindow: public QMainWindow, public qmdiHost
{
Q_OBJECT
public:
	MainWindow( QWidget *owner=NULL );

public slots:
	void init_actions();
	void init_gui();
	void about();
	void fileNew();
	void fileClose();
	
private:
	QAction *actionQuit;
	QAction *actionFileNew, *actionFileClose;
	QAction *actionAbout;
	QTabWidget *tabWidget;
};

#endif // __MAIN_WIN_H__
