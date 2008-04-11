#ifndef __HELP_BROWESER_H__
#define __HELP_BROWESER_H__

/**
 * \file helpbrowse.h
 * \brief Definition of the extended help browser class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL 2 or 3
 * \see MainWindow
 */
 
// $Id$ 

#include <QTextBrowser>
#include "qmdiclient.h"

class QAction;
class QWidget;
class QComboBox;

class QexHelpBrowser: public QTextBrowser, public qmdiClient
{
	Q_OBJECT
public:
	QexHelpBrowser( QUrl home, bool singleToolbar=false, QWidget *parent=0 );
	void initInterface( bool singleToolbar=false );

public slots:
	void goHome();
	void on_documentCombo_currentIndexChanged( int index );

private:
	QAction *actionBack;
	QAction *actionNext;
	QAction *actionHome;
	QAction *actionZoomIn;
	QAction *actionZoomOut;
	
	QAction *actionCopy;
	QAction *actionFind;

	QComboBox *documentCombo;
	QUrl homePage;
};

#endif // __HELP_BROWESER_H__
