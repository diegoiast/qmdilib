#ifndef __Q_EX_RICHTEXTBROWSER_H__
#define __Q_EX_RICHTEXTBROWSER_H__

/**
 * \file richtext_browser.h
 * \brief Definition of the QexRichTextBrowser class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL
 * \see QexRichTextBrowser
 */

// $Id: pluginmanager.h 146 2007-04-23 22:45:01Z elcuco $

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
