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
class QToolButton;

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
	void helpQtTopics();
	
private:
	QAction *actionQuit;
	QAction *actionFileNew;
	QAction *actionQtTopics;
	QAction *actionAbout;
	QTabWidget *tabWidget;

	QToolButton *tabCloseBtn;
	QToolButton *tabNewBtn;
};

#endif // __MAIN_WIN_H__
