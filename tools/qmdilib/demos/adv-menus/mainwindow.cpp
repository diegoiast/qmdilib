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


MainWindow::MainWindow( QWidget *owner ):QMainWindow(owner)
{
	QTextBrowser *browser = new QTextBrowser;
	browser->setSource( QUrl(":adv-menus.html") );
	setCentralWidget( browser );
	statusBar();

	init_actions();
	init_gui();
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
	advanced.menus["&File"]->addAction( actionFileSaveAs );
	advanced.menus["&Test"]->addAction( test1 );
	advanced.menus["&Test"]->addAction( test2 );
	advanced.menus["&Help"]->addAction( actionAboutQt );

	// extra toolbars
	advanced.toolbars["Main"]->addAction( actionQuit );
	advanced.toolbars["File operations"]->addAction( test1 );
	advanced.toolbars["File operations"]->addAction( test2 );
	
	// show the stuff on screen
	menus.updateMenu( menuBar() );
	toolBarList = toolbars.updateToolBar( toolBarList, this );
}

void MainWindow::showMenus()
{
	bool isChecked = actionShowAll->isChecked();

	if (isChecked)
	{
		menus.mergeGroupList( &advanced.menus );
		toolbars.mergeGroupList( &advanced.toolbars );
	}
	else
	{
		menus.unmergeGroupList( &advanced.menus );
		toolbars.unmergeGroupList( &advanced.toolbars );
	}

	toolBarList = toolbars.updateToolBar( toolBarList, this );
	menus.updateMenu( menuBar() );
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
