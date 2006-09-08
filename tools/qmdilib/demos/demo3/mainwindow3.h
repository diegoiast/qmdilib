#ifndef __MAIN_WIN_H__
#define __MAIN_WIN_H__

/**
 * \file mainwindow2.h
 * \brief Implementation of the main window class of the 2nd demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

//#include <QMainWindow>
#include "qmdihost.h"
#include "qmdiclient.h"
#include "qmditabwidget.h"
#include "qmdimainwindow.h"


class QTabWidget;
class QToolButton;
class QWorkspace;

//class MainWindow3: public QMainWindow, public qmdiHost
class MainWindow3: public qmdiMainWindow
{
Q_OBJECT
public:
	MainWindow3( QWidget *owner=NULL );

public slots:
	void init_actions();
	void init_gui();
	void about();
	void fileNew();
	void fileClose();
	void helpQtTopics();
	
private:
	QAction *actionQuit;
	QAction *actionFileNew;
	QAction *actionQtTopics;
	QAction *actionAbout;
	QTabWidget *tabWidget;
	QWorkspace  *workspace;

	QToolButton *tabCloseBtn;
	QToolButton *tabNewBtn;
};

#endif // __MAIN_WIN_H__
