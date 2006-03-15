#include <QAction>
#include <QString>

/**
 * \file qextexteditor.cpp
 * \brief Definition of the extended text editor class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

#include "qexeditor.h"

QexTextEdit::QexTextEdit( QWidget *parent ):QTextEdit( parent )
{
	actionSave	= new QAction( tr("&Save"), this );
	actionClose	= new QAction( tr("&Close"), this );
	actionCopy	= new QAction( tr("&Copy"), this );
	actionPaste	= new QAction( tr("&Paste"), this  );
	actionFind	= new QAction( tr("&Find"), this  );
	actionOptions	= new QAction( tr("&Options"), this  );

	connect( actionClose, SIGNAL(triggered()), this, SLOT(deleteLater()));
	actionClose->setToolTip( "Closing from the widget itself" );

	// define the menus for this widget
	menus["&File"]		->addAction( actionSave );
	menus["&File"]		->addAction( actionClose );
	menus["&Edit"]		->addAction( actionCopy );
	menus["&Search"]	->addAction( actionFind );
	menus["&Configuration"]	->addAction( actionOptions );

	// define the toolbars for this widget
	toolbars["File"]	->addAction( actionSave );
	toolbars["File"]	->addAction( actionClose );
	toolbars["Edit operations"]->addAction( actionPaste );
	toolbars["Edit operations"]->addAction( actionCopy );
}

