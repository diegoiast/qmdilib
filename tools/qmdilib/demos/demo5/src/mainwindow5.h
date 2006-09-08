#ifndef __MAIN_WIN_H__
#define __MAIN_WIN_H__

/**
 * \file mainwindow5.h
 * \brief Implementation of the main window class of the 5th demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */
 
// $Id$ 

#include <QMainWindow>
#include "qmdihost.h"
#include "qmdiclient.h"
#include "qmditabwidget.h"


class QTabWidget;
class QToolButton;

class MainWindow5: public QMainWindow, public qmdiHost
{
Q_OBJECT
public:
	MainWindow5( QWidget *owner=NULL );

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
