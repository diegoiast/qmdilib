#include <QIcon>
#include <QAction>
#include "helpbrowse.h"

/**
 * \file helpbrowse.cpp
 * \brief Implementation of the extended help browser class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

QexHelpBrowser::QexHelpBrowser( QUrl home, QWidget *parent )
	:QTextBrowser(parent)
{
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

	actionCopy->setEnabled(false);
	actionNext->setShortcut( QKeySequence("Alt+Right") );
	actionBack->setShortcut( QKeySequence("Alt+Left") );
	actionHome->setShortcut( QKeySequence("Alt+Home") );
	actionZoomIn->setShortcut( QKeySequence("Ctrl++") );
	actionZoomOut->setShortcut( QKeySequence("Ctrl+-") );

	// define the menus for this widget
	menus["&Edit"]		->addAction( actionCopy );
	
	menus["&Navigation"]->addAction( actionHome );
	menus["&Navigation"]->addAction( actionBack );
	menus["&Navigation"]->addAction( actionNext );
	menus["&Navigation"]->addSeparator();
	menus["&Navigation"]->addAction( actionZoomIn );
	menus["&Navigation"]->addAction( actionZoomOut );

	// define the toolbars for this widget
	toolbars["Edit operations"]->addAction( actionCopy );
	toolbars["Navigation"]->addAction( actionHome );
	toolbars["Navigation"]->addAction( actionBack );
	toolbars["Navigation"]->addAction( actionNext );
	toolbars["Navigation"]->addSeparator();
	toolbars["Navigation"]->addAction( actionZoomIn );
	toolbars["Navigation"]->addAction( actionZoomOut );
	
	homePage = home;
	setSource( homePage );
}

void QexHelpBrowser::goHome()
{
	setSource( homePage );
}
