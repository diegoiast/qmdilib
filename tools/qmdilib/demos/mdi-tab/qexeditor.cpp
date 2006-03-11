#include <QAction>
#include <QString>

#include "qexeditor.h"

QexTextEdit::QexTextEdit( QWidget *parent ):QTextEdit( parent )
{
	actionCopy	= new QAction( tr("&Copy"), this );
	actionPaste	= new QAction( tr("&Paste"), this  );
	actionFind	= new QAction( tr("&Find"), this  );
	actionOptions	= new QAction( tr("&Options"), this  );

	// define the menus for this widget
	menus["&Edit"]		->addAction( actionPaste );
	menus["&Edit"]		->addAction( actionCopy );
	menus["&Search"]	->addAction( actionFind );
	menus["&Configuration"]	->addAction( actionOptions );

	// define the toolbars for this widget
	toolbars["Edit operations"]->addAction( actionPaste );
	toolbars["Edit operations"]->addAction( actionCopy );
}
