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
	QFont font;
	font.setFamily("Courier New");
	font.setPointSize(10);
	font.setFixedPitch(true);
	setFont( font );
	setAcceptRichText( false );
	setLineWrapMode( QTextEdit::WidgetWidth );
	
	actionSave	= new QAction( QIcon(":images/save.png"), tr("&Save"), this );
	actionClose	= new QAction( QIcon(":images/fileclose.png"), tr("&Close"), this );
	actionUndo	= new QAction( QIcon(":images/redo.png"), tr("&Redo"), this );
	actionRedo	= new QAction( QIcon(":images/undo.png"), tr("&Undo"), this );
	actionCopy	= new QAction( QIcon(":images/copy.png"), tr("&Copy"), this );
	actionCut	= new QAction( QIcon(":images/cut.png"), tr("&cut"), this );
	actionPaste	= new QAction( QIcon(":images/paste.png"), tr("&Paste"), this  );
	actionFind	= new QAction( QIcon(":images/find.png"), tr("&Find"), this  );

	connect( this, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)) );
// 	connect( this, SIGNAL(pasteAvailable(bool)), actionPaste, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)) );
	
        connect( actionUndo, SIGNAL(triggered()), this, SLOT(undo()) );
        connect( actionRedo, SIGNAL(triggered()), this, SLOT(redo()) );
        connect( actionCopy, SIGNAL(triggered()), this, SLOT(copy()) );
        connect( actionCut, SIGNAL(triggered()), this, SLOT(cut()) );
        connect( actionPaste, SIGNAL(triggered()), this, SLOT(paste()) );
	connect( actionClose, SIGNAL(triggered()), this, SLOT(deleteLater()));

	actionUndo->setEnabled( false );
	actionRedo->setEnabled( false );
	actionCopy->setEnabled( false );
	actionCut->setEnabled( false );
	actionPaste->setEnabled( false );
	actionClose->setToolTip( "Closing from the widget itself" );

	// define the menus for this widget
	menus["&File"]		->addAction( actionSave );
	menus["&File"]		->addAction( actionClose );
	menus["&Edit"]		->addAction( actionUndo );
	menus["&Edit"]		->addAction( actionRedo );
	menus["&Edit"]		->addSeparator();
	menus["&Edit"]		->addAction( actionCopy );
	menus["&Edit"]		->addAction( actionCut );
	menus["&Edit"]		->addAction( actionPaste );
	menus["&Search"]	->addAction( actionFind );

	// define the toolbars for this widget
	toolbars["File"]	->addAction( actionSave );
	toolbars["File"]	->addAction( actionClose );
	toolbars["Edit operations"]->addAction( actionCopy );
	toolbars["Edit operations"]->addAction( actionCut );
	toolbars["Edit operations"]->addAction( actionPaste );
	toolbars["Edit operations"]->addSeparator();
	toolbars["Edit operations"]->addAction( actionUndo );
	toolbars["Edit operations"]->addAction( actionRedo );
}

