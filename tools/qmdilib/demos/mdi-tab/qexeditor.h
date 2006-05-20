#ifndef __QEX_EDITOR_H__
#define __QEX_EDITOR_H__

/**
 * \file qextexteditor.h
 * \brief Definition of the extended text editor class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

#include <QTextEdit>

#include "qmdiclient.h"

class QString;
class QTextEdit;
class QToolBar;
class QAction;
class QTextCodec;

class QexTextEdit : public QTextEdit, public qmdiClient
{
	Q_OBJECT
public:
	QexTextEdit( QString file="", QWidget *parent=0 );
	bool canCloseClient();
	bool openFile( QString newFile, QTextCodec *c=NULL  );
	bool saveFile( QString newFile, QTextCodec *c=NULL  );
	
public slots:
	bool fileSave();
	bool fileSaveAs();
	bool fileClose();
	void helpShowHelp();

private:
	QAction *actionSave;
	QAction *actionClose;

	QAction *actionUndo;
	QAction *actionRedo;	
	QAction *actionCopy;
	QAction *actionCut;
	QAction *actionPaste;
	QAction *actionFind;
	QAction *actiohAskHelp;

	QString fileName;
};

#endif //__QEX_EDITOR_H__
