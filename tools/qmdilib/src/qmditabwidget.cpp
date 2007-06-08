/**
 * \file qmditabwidget.cpp
 * \brief Implementation of the qmdi tab widget
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiServer, QTabWidget
 */
 
// $Id$

#include <QEvent>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include <QMainWindow>
#include <QWorkspace>

#include "qmditabwidget.h"
#include "qmdihost.h"
#include "qmdiclient.h"
#include "qmditabbar.h"

/**
 * \class qmdiTabWidget
 * \brief An advanced tab widget, which is capable of changing menus and toolbars on the fly
 * 
 * This is a derived class from QTabWidget which is capable of modifying the window
 * menus and toolbars according to the widgets selected. This new tab widget,
 * will connect the lower level object (the qmdiClient, the widgets inserted into the tabs) to the
 * higher level object (the qmdiHost, usually the main window). Since qmdiTabWidget inherits
 * QTabWidget you can use it as a normal QTabWidget, and all the interfaces available on the
 * base class are available for this new class.
 * 
 * The relations are:
 *  - qmdiHost   : main window
 *  - qmdiClient : your new widgets
 *  - qmdiServer : this class
 *
 * When a new widget is selected on the qmdiServer (the user changes), the old
 * widget is removed from the qmdiHost, and only then the new MDI client is added
 * to the qmdiHost.
 *  
 * To use this class properly, insert it into a QMainWindow which also derives qmdiHost,
 * and insert into it QWidgets which also derive qmdiClient.
 */

/**
 * \brief default constructor
 * \param parent the parent widget and the qmdiHost
 * \param host the default MDI host to modify
 * 
 * This is the default constructor for qmdiTabWidget.
 * If no host is passed, the parent widget will be queried for the qmdiHost
 * interface. This means that the easiest way to work with this
 * class is to insert it into a qmdiHost derived QMainWindow.
 * 
 * This constructor also connects the tabChanged(int) slot to the currentChanged(int) signal.
 * 
 * \see QWidget::parentWidget()
 */
qmdiTabWidget::qmdiTabWidget( QWidget *parent, qmdiHost *host )
	: QTabWidget( parent )
{
	if (host == NULL)
		mdiHost = dynamic_cast<qmdiHost*>(parent);
	else
		mdiHost = host;
	
	activeWidget = NULL;
	
	connect( this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	QTabBar *b = new qmdiTabBar;
	connect( b, SIGNAL(middleMousePressed(int,QPoint)), this, SLOT(on_middleMouse_pressed(int,QPoint)));
	connect( b, SIGNAL(rightMousePressed(int,QPoint)) , this, SLOT(on_rightMouse_pressed(int,QPoint)));
	setTabBar( b );
}

qmdiTabWidget::~qmdiTabWidget()
{
}

/**
 * \brief callback function for modifying the menu structure
 * \param i the number of the new widget
 *
 * When the user changes the active tab this this slot gets called.
 * It removes the menus and toolbars of the old MDI client and
 * installs the ones of the new client on screen.
 *
 * Since version 0.0.3 this widget also supports adding QWorkspace.
 * When the widget in focus is a QWorkspace, it's children will be
 * treated like normal qmdiClient if they provide that interface,
 * and when you select a new window in the QWorkspace that window's
 * menus and toolbars will be merged into the main application.
 * 
 * \see QTabWidget::currentChanged()
 * \see wSpaceWindowActivated(QWidget*)
 */
void qmdiTabWidget::tabChanged( int i )
{
	if (mdiHost == NULL)
		mdiHost = dynamic_cast<qmdiHost*>(parentWidget());
		
	if (mdiHost == NULL)
		return;
		
	QWidget *w = widget( i );

	// nothing to do, if the same tab has been selected twice
	if (w == activeWidget)
		return;

	// thanks to Nox PasNox <pasnox@gmail.com> for this code
	// it enables the usage of QWorspace as a self contained qmdiServer
	if (activeWidget)
	{
		mdiHost->unmergeClient( dynamic_cast<qmdiClient*>(activeWidget) );
		QWorkspace *ws = qobject_cast<QWorkspace*>( activeWidget );
		if ( ws )
			mdiHost->unmergeClient( dynamic_cast<qmdiClient*>(ws->activeWindow()) );
	}
	
	activeWidget = w;
	
	if (activeWidget)
	{
		mdiHost->mergeClient( dynamic_cast<qmdiClient*>(activeWidget) );
		QWorkspace *ws = qobject_cast<QWorkspace*>( activeWidget );
		if ( ws )
			mdiHost->mergeClient( dynamic_cast<qmdiClient*>(ws->activeWindow()) );
				
	}

	QMainWindow *m = dynamic_cast<QMainWindow*>(mdiHost);
	mdiHost->updateGUI( m );	
}

/**
 * \brief callback function for notifying of a new QWorkspace child activated
 * \param w the widget which now has the focus on the work space
 *
 * Since qmdilib 0.0.3 it's possible to insert a QWorkspace into 
 * a QTabWidget and when a new child will be focused on the QWorkspace 
 * it's menus will be merged, just like it a normal widget on the qmdiTabWidget.
 * 
 * This slow it automatically connected by tabInserted(int).
 *
 * Thanks to Nox PasNox <pasnox@gmail.com> for this code.
 *
 * \since 0.0.3
 * \see tabInserted( int )
 * \see tabChanged( int )
 */
void qmdiTabWidget::wSpaceWindowActivated( QWidget* w )
{
	static qmdiClient* last = NULL;

	if (mdiHost == NULL)
		mdiHost = dynamic_cast<qmdiHost*>(parentWidget());
		
	if (mdiHost == NULL)
		return;
	
	QWorkspace* workspace = qobject_cast<QWorkspace*>( sender() );
	if (workspace == NULL)
		return;

	QWidgetList l = workspace->windowList();
	qmdiClient *client = dynamic_cast<qmdiClient*>( w );
	mdiHost->unmergeClient( last );
	mdiHost->mergeClient( client );
	last = client;

	QMainWindow *m = dynamic_cast<QMainWindow*>(mdiHost);
	mdiHost->updateGUI( m );
}

/**
 * \brief mouse middle button click callback
 * \param i number of client pressed
 * 
 * This function is connected to the mouse middle click even
 * on the tabbar. It will try to close the client.
 * 
 * \see qmdiServer::tryCloseClient
 */
void qmdiTabWidget::on_middleMouse_pressed( int i, QPoint  )
{
	tryCloseClient( i );
}

/**
 * \brief mouse right button click callback
 * \param i number of client pressed
 * \param p coordinate of the click event
 * 
 * This function is connected to the mouse right click even
 * on the tabbar. This function will display a popup menu.
 * 
 * \see qmdiServer::showClientMenu
 */
void qmdiTabWidget::on_rightMouse_pressed( int i, QPoint p )
{
	showClientMenu( i, p );
}

/**
 * \brief add a new MDI client to this tab widget
 * \param client the new client to be added
 *
 * This function is demanded by qmdiServer, and is implemented
 * as a simple call to:
 *
 * \code
 * i = QTabWidget::addTab( client, client->name );
 * QTabWidget::setCurrentIndex( i );
 * client->setFocus();
 * \endcode
 * 
 * The client must derive also QWidget, since only widgets can
 * be inserted into QTabWidget. If the client does not derive
 * QWidget the function returns without doing anything.
 */
void qmdiTabWidget::addClient( qmdiClient *client )
{
	QWidget *w = dynamic_cast<QWidget*>(client);

	if (w == NULL)
	{
		qDebug( "%s %s %d: warning trying to add a qmdiClient which does not derive QWidget", __FILE__, __FUNCTION__, __LINE__ );
		return;
	}
	
	int i = addTab( w, client->name );
	w->setFocus();
	setCurrentIndex( i );
}

/**
 * \brief return the number of sub clients in this server
 * \param i the number of sub widget to return
 *
 * Return the number of sub-widgets in this server. Please note that
 * this function can return also non-mdi clients. 
 *
 * This function return the value or QTabWidget::widget(i)
 */
qmdiClient *qmdiTabWidget::getClient( int i )
{
	return dynamic_cast<qmdiClient*>( widget(i) );
}

/**
 * \brief return the number of sub clients in this server
 *
 * Return the number of sub-widgets in this server. Please note that
 * this function can return also non-mdi clients. 
 *
 * This function return the value or QTabWidget::count()
 */
int qmdiTabWidget::getClientsCount()
{
	return count();
}

/**
 * \brief callback to get alarm of deleted object
 * \param client the client to delete
 * 
 * As requested by qmdiServer this function implements the needed
 * interface. When an object is deleted, eighter by QTabWidget::removeTab(int), or
 * by deleting the object, this function will be called.
 * 
 * This function removes the menus and toolbars of the widget (if it is the
 * active widget) and sets the active widget to NULL. When a new tab will be selected,
 * which will happen if there is another widget on the tab widget, the new client 
 * will be merged.
 * 
 * \see qmdiServer::clientDeleted( QObject * )
 */
void qmdiTabWidget::deleteClient( qmdiClient* client )
{
	if (client == NULL)
		return;
		
	if (mdiHost == NULL)
		return;
	
	if (dynamic_cast<qmdiClient*>(activeWidget) != client)
		return;
	
	QWorkspace* ws = qobject_cast<QWorkspace*>( activeWidget );
	if (ws)
		foreach ( QWidget* c, ws->windowList() )
			mdiHost->unmergeClient( dynamic_cast<qmdiClient*>(c) );
	
	mdiHost->unmergeClient( client );
	mdiHost->updateGUI( dynamic_cast<QMainWindow*>(mdiHost) );
	activeWidget = NULL;
}

/**
 * \brief callback for getting informed of new MDI clients
 * \param index the index of the new widget
 *
 * This function will be called when the a new tab is inserted
 * into the tab widget. This sets the MDI server property of the qmdiClient
 * to \b this, which is needed to call deleteClient().
 * 
 * If this is the only widget on the tab widget it generates a call to tabChanged()
 * to update the menus and toolbars. If there are more then 1 widget 
 * the call will be generated by Qt for us. 
 * 
 * \bug Is this a bug in Qt...?
 * \see QTabWidget::tabInserted( int )
 */
void qmdiTabWidget::tabInserted ( int index )
{
	QWidget *w = widget( index );
	qmdiClient *client = dynamic_cast<qmdiClient*>(w);

	if (client)
		client->mdiServer = this;

	QWorkspace* ws = qobject_cast<QWorkspace*>( w );
	if (ws)
		connect( ws, SIGNAL(windowActivated(QWidget*)), this, SLOT( wSpaceWindowActivated(QWidget*)));

//	if it's the only widget available, show it's number
//	BUG is this supposed to be done by Qt?
	if (count() == 1)
	{
		tabChanged( 0 );	// TODO: is this needed...?
		activeWidget = w;
	}
}

/**
 * \brief callback for getting informed of removed MDI client
 * \param index the index of the new widget
 *
 * This function will be called when a tab is removed. The MDI client
 * will un-merge itself on it's destructor, however if it was the only
 * widget available (and the tab widget is now empty), the GUI needs
 * to be updated as the tabChanged() function will not get called.
 * 
 * This function will be called \b after the widget has been deleted, and thus
 * widget(index) is not the deleted widget! For this reason the qmdiClient must
 * un-merge itself - the MDI server has no way of knowing why object has been
 * deleted.
 * 
 * \see QTabWidget::tabRemoved( int )
 */
void qmdiTabWidget::tabRemoved ( int index )
{
	if (mdiHost == NULL)
		return;

	if (count() == 0)
	{
		activeWidget = NULL;

		// the deletion of menus and toolbars is made by qmdiClient itself
		mdiHost->updateGUI( dynamic_cast<QMainWindow*>(mdiHost) );
	}

	// this is done to shut up gcc warnings
	index = 0;
	
//	if it's the only widget available, show it's number
//	BUG is this supposed to be done by Qt?
	if (count() == 1)
		tabChanged( 0 );
}
