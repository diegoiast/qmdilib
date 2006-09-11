/**
 * \file helpbrowse.cpp
 * \brief Implementation of the extended help browser class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */
 
// $Id$

#include <QIcon>
#include <QAction>
#include <QComboBox>

#include "helpbrowse.h"

/**
 * \class QexHelpBrowser
 * \brief a small help browser based on QTextBrowser
 *
 * This class demostrates how to make a small help browser
 * using QTextBrowser from the Qt library and qmdilib.
 * 
 * The class contains the basic next/previous/home actions,
 * as well as zoom in and out. IT full integrates into the host 
 * application, by adding context menus and toolbars when the 
 * widget is selected on a qmdiTabWidget.
 * 
 * The class is a qmdiClient and all the menus and toolbars are defined
 * using that interface.
 * 
 * \see qmdiClient
 */

QexHelpBrowser::QexHelpBrowser( QUrl home, bool singleToolbar, QWidget *parent )
	:QTextBrowser(parent)
{
	zoomCombo = new QComboBox;
	zoomCombo->addItem( tr("Smaller text"), -5 );
	zoomCombo->addItem( tr("Bigger text"), 5 );
	
	actionBack	= new QAction( QIcon(":images/prev.png"), tr("&Back"), this );
	actionNext	= new QAction( QIcon(":images/next.png"), tr("&Next"), this );
	actionHome	= new QAction( QIcon(":images/home.png"), tr("&Home"), this );
	actionCopy	= new QAction( QIcon(":images/copy.png"), tr("&Copy"), this  );
	actionZoomIn	= new QAction( QIcon(":images/zoomin.png"), tr("&Zoom in"), this  );
	actionZoomOut	= new QAction( QIcon(":images/zoomout.png"), tr("&Zoom out"), this  );

	connect( this, SIGNAL(backwardAvailable(bool)), actionBack, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(forwardAvailable(bool)), actionNext, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)) );
	connect( actionBack, SIGNAL(triggered()), this, SLOT(backward()));
	connect( actionNext, SIGNAL(triggered()), this, SLOT(forward()));
	connect( actionHome, SIGNAL(triggered()), this, SLOT(goHome()));
	connect( actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
	connect( actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
	connect( zoomCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_zoomCombo_currentIndexChanged(int)));

	actionCopy->setEnabled(false);
	actionNext->setShortcut( QKeySequence("Alt+Right") );
	actionBack->setShortcut( QKeySequence("Alt+Left") );
	actionHome->setShortcut( QKeySequence("Alt+Home") );
	actionZoomIn->setShortcut( QKeySequence("Ctrl++") );
	actionZoomOut->setShortcut( QKeySequence("Ctrl+-") );

	
	initInterface( singleToolbar );
	homePage = home;
	setSource( homePage );
}

void QexHelpBrowser::initInterface( bool singleToolbar )
{
	QString toolbarFile = singleToolbar? "main" : "File";
	QString toolbarEdit = singleToolbar? "main" : "Edit operations";
	QString toolbarNavigate = singleToolbar? "main" : "Navigation";
	setFrameStyle( QFrame::NoFrame );
        setContentsMargins(0, 0, 0, 0);


	// define the menus for this widget
	menus["&Edit"]		->addAction( actionCopy );
	
	menus["&Navigation"]->addAction( actionHome );
	menus["&Navigation"]->addAction( actionBack );
	menus["&Navigation"]->addAction( actionNext );
	menus["&Navigation"]->addSeparator();
	menus["&Navigation"]->addAction( actionZoomIn );
	menus["&Navigation"]->addAction( actionZoomOut );

	// define the toolbars for this widget
	if (singleToolbar)
		toolbars[ toolbarFile ]->addSeparator();
	toolbars[ toolbarEdit ]->addAction( actionCopy );
	
	if (singleToolbar)
		toolbars[ toolbarNavigate ]->addSeparator();
	toolbars[ toolbarNavigate ]->addAction( actionHome );
	toolbars[ toolbarNavigate ]->addAction( actionBack );
	toolbars[ toolbarNavigate ]->addAction( actionNext );
	toolbars[ toolbarNavigate ]->addSeparator();
	toolbars[ toolbarNavigate ]->addAction( actionZoomIn );
	toolbars[ toolbarNavigate ]->addAction( actionZoomOut );
	toolbars[ toolbarNavigate ]->addWidget( zoomCombo );
}

void QexHelpBrowser::goHome()
{
	setSource( homePage );
}

void QexHelpBrowser::on_zoomCombo_currentIndexChanged( int index )
{
	zoomIn( 0 );
	zoomIn( zoomCombo->itemData(index).toInt() );
}