#ifndef __Q_EX_RICHTEXTBROWSER_H__
#define __Q_EX_RICHTEXTBROWSER_H__

#include "qmdiclient.h"
#include "richtext.h"

class QexRichTextBrowser: public RichText, public qmdiClient
{
	Q_OBJECT
public:
	QexRichTextBrowser( QString fileName, QWidget *parent );
	bool canCloseClient();
	
private:
	QTextEdit *edit;	
};

#endif //__Q_EX_RICHTEXTBROWSER_H__
