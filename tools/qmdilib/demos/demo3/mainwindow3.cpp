/**
 * \file MainWindow3.cpp
 * \brief Implementation of the main window class of the 3rd demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

// $Id$

#include <QAction>
#include <QToolBar>
#include <QMainWindow>
#include <QTextEdit>
#include <QTabWidget>
#include <QTextBrowser>
#include <QMessageBox>
#include <QToolButton>
#include <QUrl>
#include <QLibraryInfo>
#include <QApplication>
#include <QWorkspace>

#include "mainwindow3.h"
#include "qexeditor.h"
#include "helpbrowse.h"



/**
 * \class MainWindow3
 * \brief a window with an qmdiTabWidget and 
 *
 * This example demostrates how to use qmdiTabWidget, and
 * define different qmdiClient. It shows you how can you insert 
 * a normal QWorkspace into the mdi server. All menus and toolbars 
 * of the worksapce will be merged, if they derive qmdiClient.
 */

#define SINGLE_TOOLBAR false

MainWindow3::MainWindow3( QWidget *owner ):qmdiMainWindow(owner)
{
	statusBar();
	init_actions();
	init_gui();
}

void MainWindow3::init_actions()
{
	actionQuit = new QAction( QIcon(":images/quit.png"), "&Quit", this );
	actionQuit->setShortcut( QKeySequence("Ctrl+Q") );
	connect( actionQuit, SIGNAL(triggered()), this, SLOT(close()) );

	actionFileNew = new QAction( QIcon(":images/new.png"), "&New...", this );
	actionFileNew->setShortcut( QKeySequence("Ctrl+N") );
	connect( actionFileNew, SIGNAL(triggered()), this, SLOT(fileNew()) );

	actionQtTopics = new QAction( QIcon(":images/qt-logo.png"), "&Qt Help", this );
	connect( actionQtTopics, SIGNAL(triggered()), this, SLOT(helpQtTopics()) );

	actionAbout = new QAction( "&About", this );
	connect( actionAbout, SIGNAL(triggered()), this, SLOT(about()) );
}

void MainWindow3::init_gui()
{
	// create own menus
	menus["&File"]->addAction( actionFileNew );
	menus["&File"]->addSeparator();
	menus["&File"]->addAction( actionQuit );
	menus["&Edit"];
	menus["&Navigation"];
	menus["&Search"];
	menus["&Configuration"];
	menus["&Help"]->addAction( actionQtTopics );
	menus["&Help"]->addAction( actionAbout );

	// toolbars
	toolbars["main"]->addAction( actionFileNew );
	toolbars["main"]->addAction( actionQtTopics );

	// show the stuff on screen
	updateGUI( this );

	// make the tab widget
	tabWidget = new qmdiTabWidget;
	
	tabNewBtn = new QToolButton(tabWidget);
        tabNewBtn->setAutoRaise( true );
        connect( tabNewBtn, SIGNAL(clicked()), this, SLOT(fileNew()));
	tabNewBtn->setIcon(QIcon(":images/addtab.png"));

	tabCloseBtn = new QToolButton(tabWidget);
        tabCloseBtn->setAutoRaise( true );
        connect( tabCloseBtn, SIGNAL(clicked()), this, SLOT(fileClose()));
	tabCloseBtn->setIcon(QIcon(":images/closetab.png"));

	tabWidget->setCornerWidget( tabNewBtn, Qt::TopLeftCorner );
	tabWidget->setCornerWidget( tabCloseBtn, Qt::TopRightCorner  );
		
	// feed it with a default widget, this browser is a
	// non mdi client, and will add no new menus nor toolbars
	QTextBrowser *browser = new QTextBrowser;
	browser->setObjectName("welcome_tab");
	browser->setSource( QUrl( QApplication::applicationDirPath() + "/demo3.html" ) );
        browser->setFrameStyle( QFrame::NoFrame );
        browser->setContentsMargins(0, 0, 0, 0);	
	tabWidget->addTab( browser, "Welcome" );

	workspace = new QWorkspace;
	tabWidget->addTab( workspace, "Workspace" );

	setCentralWidget( tabWidget );
}

void MainWindow3::about()
{
	QMessageBox::about(NULL, "About Program",
		"This demo is part of the qmdi library.\nDiego Iasturbni <elcuco@kde.org> - LGPL"
	);
}

void MainWindow3::fileNew()
{
	QexTextEdit *editor = new QexTextEdit( SINGLE_TOOLBAR );
	editor->hide();
	workspace->addWindow( editor );
	editor->show();
}

void MainWindow3::fileClose()
{
	qmdiClient *c = dynamic_cast<qmdiClient*>(tabWidget->currentWidget());

	if (c == NULL)
		// if it's not an mdi client, safe to kill it
		delete tabWidget->currentWidget();
	else
		// otherwise, ask politelly for it to close it
		c->closeClient();
}

void MainWindow3::helpQtTopics()
{
	QString helpFile = QLibraryInfo::location(QLibraryInfo::DocumentationPath) + QLatin1String("/html/index.html");
	QexHelpBrowser *browser = new QexHelpBrowser( QUrl("file:" + helpFile), SINGLE_TOOLBAR );
	browser->hide();
//	tabWidget->addTab( browser, "Qt help" );
	workspace->addWindow( browser );
	browser->show();
}
