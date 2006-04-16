#ifndef __EDITOR_PLUGIN_H__
#define __EDITOR_PLUGIN_H__

#include "iplugin.h"

class QAction;

class EditorPlugin: public IPlugin
{
	Q_OBJECT
public:
	EditorPlugin();
	~EditorPlugin();

	void showAbout();

public slots:
	void fileNew();
	void fileOpen();
	
private:
	QAction *actionNew;
	QAction *actionOpen;
};

#endif// __EDITOR_PLUGIN_H__
