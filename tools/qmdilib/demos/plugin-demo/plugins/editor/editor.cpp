#include <QAction>
#include <QMessageBox>

#include "iplugin.h"
#include "qmdiserver.h"
#include "editor.h"
#include "qexeditor.h"


EditorPlugin::EditorPlugin()
{
	actionNew	= new_action( QIcon(":images/filenew.png"), tr("&New"), this, tr("Ctrl+N"), tr("Create a new file"), SLOT(fileNew()) );
	actionOpen	= new_action( QIcon(":images/filenew.png"), tr("&Open"), this, tr("Ctrl+O"), tr("Load a file"), SLOT(fileOpen()) );

	name = tr("Text editor plugin");
	author = tr("Diego Iastrubni <elcuco@kde.org>");
	iVersion = 0;
	sVersion = "0.0.1";
	autoEnabled = true;
	alwaysEnabled = false;

	menus["&File"]->addAction( actionNew );
	menus["&File"]->addAction( actionOpen );
}

EditorPlugin::~EditorPlugin()
{
	delete actionNew;
	delete actionOpen;
}

void EditorPlugin::showAbout()
{
	QMessageBox::about( NULL, tr("Text Editor plugin"), tr("This plugin brings text editing capabilities to the application") );
}

void EditorPlugin::fileNew()
{
	if (!mdiServer)
	{
		qDebug("%s - warning no mdiServer defined", __FUNCTION__ );
		return;
	}

	QexTextEdit *editor = new QexTextEdit;
	editor->hide();
	editor->name = "No name";
	editor->setObjectName( editor->name );
	mdiServer->addClient( editor );
}

void EditorPlugin::fileOpen()
{
}
