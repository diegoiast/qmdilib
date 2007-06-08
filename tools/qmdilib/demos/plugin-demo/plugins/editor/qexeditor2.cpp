#include <QIcon>
#include <QAction>
#include <QTextCursor>
#include <QAction>
#include <QMainWindow>
#include <QStatusBar>

#include "qexeditor2.h"
#include "pluginmanager.h"
#include "qmdiserver.h"

QexTextEdit2::QexTextEdit2( QString file, bool singleToolbar, QWidget *parent )
	:QexTextEdit( file, singleToolbar, parent )
{
	actionShowQtKeyword = new QAction( QIcon(":images/copy.png"), tr("&Show keywork documentation"), this );
	actionShowQtKeyword->setShortcut( tr("F1") );
	
	menus["&File"]->removeAction( actionClose );
	menus["&Help"]->addAction( actionShowQtKeyword );
	
	connect( actionShowQtKeyword, SIGNAL(triggered()), this, SLOT(showQtKeyword()) );
}

void QexTextEdit2::showQtKeyword()
{
	if (!mdiServer)
		return;
	
	if (!mdiServer->mdiHost)
		return;
	
	PluginManager *m = dynamic_cast<PluginManager*>(mdiServer->mdiHost);
	QMainWindow   *w = qobject_cast<QMainWindow*>(this->window());
	
	QTextCursor cursor = textCursor();
	cursor.select(QTextCursor::WordUnderCursor);
	if (!m->openFile( "help:" + cursor.selectedText().toLower() ))	
	{
		// lets inform the user
		if (w)
			w->statusBar()->showMessage( "Could not find help for this keyword", 5 * 1000 ); // this is msec
	}
}
