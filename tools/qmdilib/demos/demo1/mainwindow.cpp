#include <QAction>
#include <QToolBar>
#include <QMainWindow>
#include <QTextEdit>
#include <QTabWidget>
#include <QTextBrowser>
#include <QMessageBox>

#include "mainwindow.h"

/**
 * \file mainwindow.cpp
 * \brief Implementation of the main window class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

/**
 * \class MainWindow
 * \brief The main window of demo 1 - modify main menus on the fly
 *
 * This demostration shows how can you had and remove menus
 * and toolbars on an application, on response to application
 * events.
 *
 * This is done by adding a new mdi client and then merging and
 * unmerging this on demand. After a merge new toolbars and menus
 * will be seen on screen. When the event is called again, the new
 * mdi client is unmerged, and the original menus are seen.
 *
 * The basic concept of this application, is:
 *   - a main window derived from QMainWindow and qmdiHost
 *   - on the main window, the menus and toolbars are defined
 *     using the APIs defined in qmdiClient
 *   - a new qmdiClient class is defined
 *   - new menus and toolbars are defined using the APIs
 *     defined in qmdiClient
 *   - when the user chooses the "Advanced menus" the 
 *     new qmdiClient is merged into the main window,
 *     and the gui is updated.
 *
 * On this application there are also defined also some NULL
 * QAction defined, just to fill up the menus and toolbars. They
 * could easily be connected to real slots.
 *
 * There is more documentation available at the adv-menus.html
 * file, which is displayed when the application starts.
 */

MainWindow::MainWindow( QWidget *owner ):QMainWindow(owner)
{
	QTextBrowser *browser = new QTextBrowser;	
	browser->setSource( QUrl(":adv-menus.html") );
	setCentralWidget( browser );
	statusBar();

	init_actions();
	init_gui();
}

MainWindow::~MainWindow()
{
	delete advanced;
}


void MainWindow::init_actions()
{
	actionShowAll = new QAction( "Advanced menus", this );
	actionShowAll->setCheckable( true );
	actionShowAll->setShortcut( QKeySequence("F12") );
	connect( actionShowAll, SIGNAL(triggered()), this, SLOT(showMenus()));

	actionQuit = new QAction( "&Quit", this );
	actionQuit->setShortcut( QKeySequence("Ctrl+Q") );
	connect( actionQuit, SIGNAL(triggered()), this, SLOT(close()) );

	actionFileNew		= new QAction( "&New...", this );
	actionFileSave		= new QAction( "&Save..", this );
	actionFileSaveAs	= new QAction( "Save &as..", this );

	actionAbout		= new QAction( "&About", this );
	connect( actionAbout, SIGNAL(triggered()), this, SLOT(about()) );
	
	actionAboutQt		= new QAction( "&About Qt", this );
	connect( actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()) );
		
	test1 = new QAction( "Test1 ", this );
	test2 = new QAction( "Test2", this );
}

void MainWindow::init_gui()
{
	// create own menus
	menus["&File"]->addAction( actionFileNew );
	menus["&File"]->addAction( actionFileSave );
	menus["&File"]->addSeparator();
	menus["&File"]->addAction( actionQuit );
	menus["&Edit"];
	menus["&Test"];
	menus["&Settings"]->addAction( actionShowAll );
	menus["&Help"]->addAction( actionAbout );

	// toolbars
	toolbars["Main"]->addAction( actionShowAll );

	// extra menus
	advanced = new qmdiClient;
	advanced->menus["&File"]->addAction( actionFileSaveAs );
	advanced->menus["&Test"]->addAction( test1 );
	advanced->menus["&Test"]->addAction( test2 );
	advanced->menus["&Help"]->addAction( actionAboutQt );

	// extra toolbars
	advanced->toolbars["Main"]->addAction( actionQuit );
	advanced->toolbars["File operations"]->addAction( test1 );
	advanced->toolbars["File operations"]->addAction( test2 );
	
	// show the stuff on screen
	updateGUI( this );
}

void MainWindow::showMenus()
{
	bool isChecked = actionShowAll->isChecked();

	if (isChecked)
		mergeClient( advanced );
	else
		unmergeClient( advanced );

	// show the stuff on screen
	updateGUI( this );
}

void MainWindow::about()
{
	QMessageBox::about(NULL, "About Program",
		"This demo is part of the qmdi library.\nDiego Iasturbni <elcuco@kde.org> - LGPL"
	);
}

void MainWindow::aboutQt()
{
	QMessageBox::aboutQt(NULL, "About Qt");
}
