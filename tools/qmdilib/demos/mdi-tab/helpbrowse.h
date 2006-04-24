#ifndef __HELP_BROWESER_H__
#define __HELP_BROWESER_H__

/**
 * \file helpbrowse.h
 * \brief Definition of the extended help browser class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see MainWindow
 */

#include <QTextBrowser>
#include "qmdiclient.h"

class QAction;
class QWidget;
class QComboBox;

class QexHelpBrowser: public QTextBrowser, public qmdiClient
{
	Q_OBJECT
public:
	QexHelpBrowser( QUrl home, QWidget *parent=0 );

public slots:
	void goHome();
	void on_zoomCombo_currentIndexChanged( int index );

private:
	QAction *actionBack;
	QAction *actionNext;
	QAction *actionHome;
	QAction *actionZoomIn;
	QAction *actionZoomOut;
	
	QAction *actionCopy;
	QAction *actionFind;

	QComboBox *zoomCombo;
	QUrl homePage;
};

#endif // __HELP_BROWESER_H__
