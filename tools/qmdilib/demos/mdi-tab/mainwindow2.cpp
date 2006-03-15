#include <QAction>
#include <QToolBar>
#include <QMainWindow>
#include <QTextEdit>
#include <QTabWidget>
#include <QTextBrowser>
#include <QMessageBox>

#include "mainwindow2.h"
#include "qexeditor.h"

/**
 * \file mainwindow.cpp
 * \brief Implementation of the main window class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */


MainWindow::MainWindow( QWidget *owner ):QMainWindow(owner)
{
	statusBar();
	init_actions();
	init_gui();

	tabWidget = new qmdiTabWidget( this );
	setCentralWidget( tabWidget );

	QTextBrowser *browser = new QTextBrowser;
	browser->setObjectName("welcome_tab");
	browser->setSource( QUrl(":mdi-tab.html") );
	tabWidget->addTab( browser, "Welcome" );
}

void MainWindow::init_actions()
{
	actionQuit = new QAction( "&Quit", this );
	actionQuit->setShortcut( QKeySequence("Ctrl+Q") );
	connect( actionQuit, SIGNAL(triggered()), this, SLOT(close()) );

	actionFileNew = new QAction( "&New...", this );
	actionFileNew->setShortcut( QKeySequence("Ctrl+N") );
	connect( actionFileNew, SIGNAL(triggered()), this, SLOT(fileNew()) );
	
	actionFileClose = new QAction( "&Close", this );
	actionFileClose->setShortcut( QKeySequence("Ctrl+W") );
	actionFileClose->setToolTip( "Closing from the main window" );
	connect( actionFileClose, SIGNAL(triggered()), this, SLOT(fileClose()) );
	
	actionAbout		= new QAction( "&About", this );
	connect( actionAbout, SIGNAL(triggered()), this, SLOT(about()) );
}

void MainWindow::init_gui()
{
	// create own menus
	menus["&File"]->addAction( actionFileNew );
	menus["&File"]->addSeparator();
	menus["&File"]->addAction( actionQuit );
	menus["&Edit"];
	menus["&Search"];
	menus["&Configuration"];
	menus["&Help"]->addAction( actionAbout );

	// toolbars
	toolbars["File"]->addAction( actionFileNew );
	toolbars["File"]->addAction( actionFileClose );

	// show the stuff on screen
	menus.updateMenu( menuBar() );
	toolBarList = toolbars.updateToolBar( toolBarList, this );
}

void MainWindow::about()
{
	QMessageBox::about(NULL, "About Program",
		"This demo is part of the qmdi library.\nDiego Iasturbni <elcuco@kde.org> - LGPL"
	);
}

void MainWindow::fileNew()
{
	QexTextEdit *editor = new QexTextEdit;
	tabWidget->addTab( editor, "MDI Editor" );
}

void MainWindow::fileClose()
{
	delete tabWidget->currentWidget();
}
