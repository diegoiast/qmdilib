#ifndef __QEXEDITOR2_H__
#define __QEXEDITOR2_H__


#include "qexeditor.h"

class QexTextEdit2: public QexTextEdit
{
	Q_OBJECT
public:
	QexTextEdit2( QString file=QString(), bool singleToolbar = false, QWidget *parent=0 );

public slots:
	void showQtKeyword();

private:
	QAction *actionShowQtKeyword;
};

#endif // __QEXEDITOR2_H__

