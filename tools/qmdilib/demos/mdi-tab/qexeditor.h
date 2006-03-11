#ifndef __QEX_EDITOR_H__
#define __QEX_EDITOR_H__

// #include <QString>
#include <QTextEdit>

#include "qmdiclient.h"

class QString;
class QTextEdit;
class QToolBar;
class QAction;

class QexTextEdit : public QTextEdit, public qmdiClient
{
public:
	QexTextEdit( QWidget *parent=0 );

private:
	QAction *actionCopy;
	QAction *actionPaste;
	QAction *actionFind;
	QAction *actionOptions;
};

#endif //__QEX_EDITOR_H__
