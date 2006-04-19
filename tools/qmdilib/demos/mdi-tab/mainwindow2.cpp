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

#include "mainwindow2.h"
#include "qexeditor.h"
#include "helpbrowse.h"

/**
 * \file mainwindow.cpp
 * \brief Implementation of the main window class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

/**
 * \class MainWindow2
 * \brief a window with an qmdiTabWidget
 * 
 * This example demostrates how to use qmdiTabWidget, and
 * define different qmdiClient. It also shows what happens when
 * you insert a non mdi client into a qmdiTabWidget.
 */

MainWindow2::MainWindow2( QWidget *owner ):QMainWindow(owner)
{
	statusBar();
	init_actions();
	init_gui();
}

void MainWindow2::init_actions()
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

void MainWindow2::init_gui()
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
	toolbars["File"]->addAction( actionFileNew );
	toolbars["File"]->addAction( actionQtTopics );

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
	setCentralWidget( tabWidget );

	// feed it with a default widget, this browser is a
	// non mdi client, and will add no new menus nor toolbars
	QTextBrowser *browser = new QTextBrowser;
	browser->setObjectName("welcome_tab");
	browser->setSource( QUrl(":mdi-tab.html") );
	tabWidget->addTab( browser, "Welcome" );
}

void MainWindow2::about()
{
	QMessageBox::about(NULL, "About Program",
		"This demo is part of the qmdi library.\nDiego Iasturbni <elcuco@kde.org> - LGPL"
	);
}

void MainWindow2::fileNew()
{
	QexTextEdit *editor = new QexTextEdit;
	editor->hide();
	tabWidget->addTab( editor, "MDI Editor" );
}

void MainWindow2::fileClose()
{
	qmdiClient *c = dynamic_cast<qmdiClient*>(tabWidget->currentWidget());

	if (c == NULL)
		// if it's not an mdi client, safe to kill it
		delete tabWidget->currentWidget();
	else
		// otherwise, ask politelly for it to close it
		c->closeClient();
}

void MainWindow2::helpQtTopics()
{
        QString helpFile = QLibraryInfo::location(QLibraryInfo::DocumentationPath) + QLatin1String("/html/index.html");
	QexHelpBrowser *browser = new QexHelpBrowser( QUrl(helpFile) );
	browser->hide();
	tabWidget->addTab( browser, "Qt help" );
}
