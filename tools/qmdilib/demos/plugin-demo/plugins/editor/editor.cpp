#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QMainWindow>

#include "iplugin.h"
#include "qmdiserver.h"
#include "editor.h"
#include "qexeditor.h"


EditorPlugin::EditorPlugin()
{
	actionNew	= new_action( QIcon(":images/filenew.png"), tr("&New"), this, tr("Ctrl+N"), tr("Create a new file"), SLOT(fileNew()) );
	actionOpen	= new_action( QIcon(":images/fileopen.png"), tr("&Open"), this, tr("Ctrl+O"), tr("Load a file"), SLOT(fileOpen()) );

	name = tr("Text editor plugin");
	author = tr("Diego Iastrubni <elcuco@kde.org>");
	iVersion = 0;
	sVersion = "0.0.1";
	autoEnabled = true;
	alwaysEnabled = false;

	menus["&File"]->addAction( actionNew );
	menus["&File"]->addAction( actionOpen );

// 	toolbars["File"]->addAction( actionNew );
// 	toolbars["File"]->addAction( actionOpen );
	
	configUI = new QWidget;
	ui.setupUi( configUI );

	makeBackups	= false;
	showLineNumbers	= true;
	makeCurrentLine	= true;
	wordWrap	= true;
	setData();
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

QWidget* EditorPlugin::getConfigDialog()
{
	return configUI;
}

void EditorPlugin::getData()
{
	makeBackups	= ui.cbMakeBackup->isChecked();
	showLineNumbers	= ui.cbShowLineNumbers->isChecked();
	makeCurrentLine	= ui.cbMarkCurrentLine->isChecked();
	wordWrap	= ui.cbWorkWrap->isChecked();
}

void EditorPlugin::setData()
{
	ui.cbMakeBackup->setChecked( makeBackups );
	ui.cbShowLineNumbers->setChecked( showLineNumbers );
	ui.cbMarkCurrentLine->setChecked(makeCurrentLine);
	ui.cbWorkWrap->setChecked(wordWrap);

	// emit a signal and notify all editors
	// about the new settings
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
	editor->name = tr("No name");
	editor->setObjectName( editor->name );
	mdiServer->addClient( editor );
}

void EditorPlugin::fileOpen()
{
	if (!mdiServer)
	{
		qDebug("%s - warning no mdiServer defined", __FUNCTION__ );
		return;
	}

	QString s = QFileDialog::getOpenFileName(
		NULL,
	tr("Choose a file"),
		"",
		tr("Sources"	,"EditorPlugin::fileOpen: open source files")	+ " (*.c *.cpp *.cxx *.h *.hpp *.hxx *.inc);;" +
		tr("Headers",	 "EditorPlugin::fileOpen: open header files")	+ " (*.h *.hpp *.hxx *.inc);;"+
		tr("Qt project", "EditorPlugin::fileOpen: open *.pro")		+ " (*.pro *.pri);;"+
		tr("All files",  "EditorPlugin::fileOpen: open any file")	+ " (*.*)"
	);

	if (s.isEmpty())
		return;

	
	QexTextEdit *editor = new QexTextEdit( s, dynamic_cast<QMainWindow*>(mdiServer) );
	editor->hide();
	mdiServer->addClient( editor );
}
