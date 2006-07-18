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


class QTabWidget;

class MainWindow: public QMainWindow, public qmdiHost
{
Q_OBJECT
public:
	MainWindow( QWidget *owner=NULL );
	~MainWindow();

public slots:
	void init_actions();
	void init_gui();
	void showMenus();
	void about();
	void aboutQt();
	
private:
	QAction *actionQuit;
	QAction *actionShowAll;
	QAction *actionFileNew, *actionFileSave, *actionFileSaveAs;
	QAction *actionAbout, *actionAboutQt;
	QAction *test1, *test2;
	
 	qmdiClient *advanced;
};

#endif // __MAIN_WIN_H__
