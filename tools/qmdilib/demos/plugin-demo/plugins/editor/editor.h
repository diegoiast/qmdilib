#ifndef __EDITOR_PLUGIN_H__
#define __EDITOR_PLUGIN_H__

#include "iplugin.h"
#include "ui_editor_cfg.h"

class QAction;

class EditorPlugin: public IPlugin
{
	Q_OBJECT
public:
	EditorPlugin();
	~EditorPlugin();

	void showAbout();
	QWidget* getConfigDialog();
	virtual QStringList myExtensions();
	virtual int canOpenFile( const QString fileName );
	virtual bool openFile( const QString fileName, int x=-1, int y=-1, int z=-1 );
	void getData();
	void setData();

public slots:
	void fileNew();
	void fileOpen();
	
private:
	QAction		*actionNew;
	QAction		*actionOpen;
	Ui::EditorCfgUI	ui;
	QWidget		*configUI;

	bool	makeBackups;
	bool	showLineNumbers;
	bool	makeCurrentLine;
	bool	wordWrap;
	QFont	editorFont;
	int	endOfLine;
};

#endif// __EDITOR_PLUGIN_H__
