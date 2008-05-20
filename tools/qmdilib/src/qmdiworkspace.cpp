/**
 * \file qmdiworkspace.cpp
 * \brief implementation of the qmdi Workspace
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiServer
 */
 
// $Id$

#include <QWidget>
#include <QTabBar>
#include <QWorkspace>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>

#include "qmdiclient.h"
#include "qmdihost.h"
#include "qmdiworkspace.h"
// #include "qmditabbar.h"

/**
 * \class qmdiWorkspace
 * \brief An advanced work space-widget, which is capable of changing menus and toolbars on the fly
 * 
 * This class is aother MDI server, based on top of QWorkspace. It is built
 * with a similar API to qmdiTabWidget and QTabWidget. Since the API is similar
 * this means you can switch applications from QTabWidget to qmdiWorkspace with
 * very small modifications to your code.
 *
 * This class is also a valid qmdiServer, which means it will dynamically
 * modify the main window menus and toolbars when a new child is selected.
 * 
 * The relations are:
 *  - qmdiHost   : main window
 *  - qmdiClient : your new widgets
 *  - qmdiServer : this class
 *
 * When a new widget is selected on the qmdiServer (the user changes), the old
 * widget is removed from the qmdiHost, and only then the new MDI client is 
 * added to the qmdiHost.
 *
 * To use this class properly, insert it into a QMainWindow which also derives 
 * qmdiHost, and insert into it QWidgets which also derive qmdiClient.
 *
 * \since 0.0.3
 */

/**
 * \var qmdiWorkspace::mainLayout
 * 
 */

/**
 * \var qmdiWorkspace::headerLayout
 * 
 */

/**
 * \var qmdiWorkspace::tabBar
 * 
 */

/**
 * \var qmdiWorkspace::workspace
 * 
 */

/**
 * \var qmdiWorkspace::_widgetList
 * 
 */


/**
 * \var qmdiWorkspace::cornerWidget1
 * 
 */

/**
 * \var qmdiWorkspace::cornerWidget2
 * 
 */

/**
 * \var qmdiWorkspace::activeWidget
 * 
 */

	
/**
 * \brief default constructor
 * \param parent the parent of this widget
 * \param host the MDI host to connect to
 *
 * This is the default constructor of qmdiWorkspace.
 * If no host is passed, the parent widget will be queried for the qmdiHost
 * interface. This means that the easiest way to work with this
 * class is to insert it into a qmdiHost derived QMainWindow.
 *
 * The constructor also creates the tab bar, to make this widget
 * look like a QTabWidget
 */
qmdiWorkspace::qmdiWorkspace( QWidget *parent, qmdiHost *host )
	: QWidget( parent )
{
	if (host == NULL)
		mdiHost = dynamic_cast<qmdiHost*>(parent);
	else
		mdiHost = host;

	cornerWidget1 = NULL;
	cornerWidget2 = NULL;
	activeWidget  = NULL;
		
	tabBar = new QTabBar(this);
	tabBar->setDrawBase( false );
	tabBar->installEventFilter( this );	
	
	workspace = new QWorkspace;	
	mainLayout = new QVBoxLayout;
	headerLayout = new QHBoxLayout;
	headerLayout->setMargin(0);
	headerLayout->setSpacing(0);
	headerLayout->setObjectName("headerLayout");
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->setObjectName("mainLayout");
	
	tabBar->setDrawBase( false );
	headerLayout->addWidget( tabBar );
	mainLayout->addItem(headerLayout);
	mainLayout->addWidget(workspace);
	
	setLayout( mainLayout );
	connect( workspace, SIGNAL(windowActivated(QWidget*)), this, SLOT(workspaceChanged(QWidget*)));
	connect( tabBar, SIGNAL(currentChanged(int)), this, SLOT(on_tabBar_changed(int)));
}

/**
 * \brief add a new MDI client to the workspace
 * \param client the client to be added to the workspace
 *
 * Adds a MDI client to the MDI server. Internally calls
 * addTab()
 *
 * \see addTab
 */
void qmdiWorkspace::addClient( qmdiClient *client )
{
	QWidget *w = dynamic_cast<QWidget*>(client);

	if (w == NULL)
		return;		
	
	addTab( w, client->mdiClientName );
}

/**
 * \brief add a new MDI client to the workspace
 * \param widget the widget to be added to the workspace
 * \param name the name to be associated with this widget
 *
 * This function does the job of adding an MDI client to the MDI
 * server. It will do the job behind the scene of setting up
 * the MDI client, and re-parenting it as needed. The name is
 * generally the name you will see on the tabbar.
 *
 * This function is made for compability with QTabWidget.
 *
 * \see QTabWidget::addTab( QWidget*, QString )
 */
void qmdiWorkspace::addTab( QWidget *widget, QString name )
{
	if (!workspace)
		return;

	qmdiClient *client = dynamic_cast<qmdiClient*>(widget);
	if (client)
		client->mdiServer = this;
		
	widget->setParent( workspace );
	workspace->addWindow( widget );
	widget->setAttribute( Qt::WA_DeleteOnClose, true );
	tabBar->addTab( name );
	widget->show();
	_widgetList.append( widget );
	connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(windowDeleted(QObject*)));
}

/**
 * \brief returns the current widget
 * \return the currently selected widget
 *
 * This function will return the widget which is currently
 * active on the QWorkspace used internally, and the tab
 * which is currently selected.
 * 
 * This function is made for compability with QTabWidget.
 * 
 * \see QWorkspace::activeWindow()
 * \see QTabWidget::currentWidget()
 */
QWidget *qmdiWorkspace::currentWidget()
{
	if (workspace)
		return workspace->activeWindow();
	else
		return NULL;
}

/**
 * \brief return the widget found in a corner
 * \param corner corner to be queried
 * \return the widget found in a specific corner
 *
 * This function return the corner widget for the
 * desired corner.
 * 
 * This function is made for compability with QTabWidget.
 * 
 * \see QTabWidget::cornerWidget( Qt::Corner )
 */
const QWidget * qmdiWorkspace::cornerWidget ( Qt::Corner corner  )
{
	switch(corner)
	{
		case Qt::TopLeftCorner:
			return cornerWidget1;
		case Qt::TopRightCorner:
			return cornerWidget2;
		case Qt::BottomLeftCorner: 
			return cornerWidget1;
		case Qt::BottomRightCorner:
			return cornerWidget2;
	}
	return NULL;
}

/**
 * \brief set the widget at a corner of the widget
 * \param widget the widget to be stored
 * \param corner the corner in which the widget should be stored
 *
 * Set the widget to be put on one of the corners of this widget.
 * Usually used to put "close" or "new" buttons on the right or left
 * of the widget.
 *
 * This function is made for compability with QTabWidget.
 * 
 * \see QTabWidget::setCornerWidget( QWidget *, Qt:Corner )
 */
void qmdiWorkspace::setCornerWidget ( QWidget * widget, Qt::Corner corner  )
{
	// should it be done...?
	// widget->setParent( tabBar );
	
	switch(corner)
	{
		case Qt::TopLeftCorner:
			cornerWidget1 = widget;
			break;
		case Qt::TopRightCorner:
			cornerWidget2 = widget;
			break;
/// \todo only tabs on the top is fully supported, add support for other styles
		case Qt::BottomLeftCorner: 
			cornerWidget1 = widget;
			break;
		case Qt::BottomRightCorner:
			cornerWidget2  = widget;
			break;
	}
	
	// to modify the corner widgets, the best I found is:
	//  * remove header layout
	//  * reconstruct a new one
	//  * and finally add the new layout back to the main layout
	// TODO: is there a smarter way of doing this?
	setUpdatesEnabled(false);
	mainLayout->removeItem( headerLayout );
	mainLayout->removeWidget( workspace );
	
	delete headerLayout;
	headerLayout = new QHBoxLayout;
	headerLayout->setMargin(0);
	headerLayout->setSpacing(0);
	headerLayout->setObjectName("headerLayout");
	
	if (cornerWidget1)
		headerLayout->addWidget(cornerWidget1);
	headerLayout->addWidget(tabBar);
	if (cornerWidget2)
		headerLayout->addWidget(cornerWidget2);
	
	mainLayout->addItem(headerLayout);
	mainLayout->addWidget(workspace);
	setUpdatesEnabled(true);
}

/**
 * \brief return a widget by number
 * \param i the number of the widget to search
 * \return the widget with the specified index
 *
 * This function returns the widget number \b i
 * in the QWorkspace used internally.
 *
 * \see QWorkspace::windowList()
 */
QWidget* qmdiWorkspace::widget( int i )
{
	if (!workspace)
		return NULL;

	// one can never be safe enough
	if (i >= workspace->windowList().count() )
	{	// no warning on first child
		if (! ((i==0) && (workspace->windowList().isEmpty())) )
			qDebug( "%s %s %d - warning: index out of range (%d)", __FILE__,  __FUNCTION__, __LINE__, i );
		return NULL;
	}
		
	if (i < 0)
	{
		qDebug( "%s %s %d - warning: negative index", __FILE__,  __FUNCTION__, __LINE__ );
		return NULL;
	}
		
	return workspace->windowList().at( i );
}

/**
 * \brief returns the index of the current active window
 * \return a number representing the id of the current active window
 *
 * This function is made for compability with QTabWidget.
 */
int qmdiWorkspace::currentIndex()
{
	QWidget *w = workspace->activeWindow();
	return workspace->windowList().indexOf(w);
}

/**
 * \brief return the number of widgets available on the workspace
 *
 * Returns the number of widgets (or windows) available. The number
 * should be the same as the number of tabs seen on screen.
 *
 */
int qmdiWorkspace::count()
{
	if (!workspace)
		return 0;
	
	return workspace->windowList().count();
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
qmdiClient* qmdiWorkspace::getClient( int i )
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
int qmdiWorkspace::getClientsCount()
{
	return count();
}

/**
 * \brief event filter for the tabbar
 * \param obj the object which created the event
 * \param event the event to be processed
 * 
 * This function is used to catch when the user is clicking a tab.
 * On earlier version, a new class has been used. Since version 0.0.4
 * a proper event filter is used, which reduces the amount of code
 * and class count in the library.
 * 
 * The function will call the functions:
 *  - on_middleMouse_pressed
 *  - on_rightMouse_pressed
 * 
 * Future implementations might also re-order the tabs.
 * 
 * For more information read the documentation of QObject::installEventFilter.
 * 
 * \since 0.0.4
 */
bool qmdiWorkspace::eventFilter(QObject *obj, QEvent *event)
{
	if (obj != tabBar)
		return QObject::eventFilter(obj, event);
	
	if (event->type() != QEvent::MouseButtonPress)
		return QObject::eventFilter(obj, event);
	
	// compute the tab number
	QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
	QPoint position = mouseEvent->pos();
	int c = tabBar->count();
	int clickedItem = -1;

	for (int i=0; i<c; i++)
	{
		if ( tabBar->tabRect(i).contains( position ) )
		{
			clickedItem = i;
			break;
		}
	}
	
	// just in case
	if (clickedItem == -1)
		return QObject::eventFilter(obj, event);
	
	switch( mouseEvent->button() )
	{
		case Qt::LeftButton:
			return QObject::eventFilter(obj, event);
			break;
			
		case Qt::RightButton:
			on_rightMouse_pressed( clickedItem, position );
			break;
			
		case Qt::MidButton:
			on_middleMouse_pressed( clickedItem, position );
			break;
		
		default:
			return QObject::eventFilter(obj, event);
	}
	
	return true;
}

/**
 * \brief called when the active window in the workspace is changed [SLOT]
 * \param w the new window which has been focused
 *
 * This slot is called when a new window is selected on the workspace.
 * This slot will also select the new tab on the tab header as well as
 * merge the MDI client menus and toolbars.
 *
 * This slot is connected at the constructor.
 */
void qmdiWorkspace::workspaceChanged( QWidget* w )
{
	if (!mdiHost)
		return;
	
	if (!workspace)
		return;

	if (activeWidget == w)
		return;
	
	if (activeWidget)
		mdiHost->unmergeClient( dynamic_cast<qmdiClient*>(activeWidget) );

	activeWidget = w;

	if (activeWidget)
		mdiHost->mergeClient( dynamic_cast<qmdiClient*>(activeWidget) );

	// try to update the MDI host
	QMainWindow *m = dynamic_cast<QMainWindow*>(mdiHost);

	if (mdiHost)
		mdiHost->updateGUI( m );
	
	// update the tab bar
	int windowNumber = workspace->windowList().indexOf(w);
	if (windowNumber!=-1)
		tabBar->setCurrentIndex( windowNumber );
}

/**
 * \brief called when the a new tab bar changed [SLOT]
 * \param index the index of the client which was focused
 *
 * This slot is called when the user selects a new tab on the
 * tab header. It will select the corresponding window/widget
 * in the workspace. This function will call will workspaceChanged()
 * to ensure that the corresponding menus and toolbars are
 * merged in.
 *
 * This slot is connected at the constructor.
 * 
 * \see workspaceChanged( QWidget* )
 */
void qmdiWorkspace::on_tabBar_changed( int index )
{
	if (!workspace)
		return;

	// one can never be safe enough
	if (index >= workspace->windowList().count() ){
		// no warning on first childs
		if (! ((index==0) && (workspace->windowList().isEmpty())) ) 
			qDebug( "%s %s %d - warning: index out of range (%d)", __FILE__,  __FUNCTION__, __LINE__, index );
		return;		
	}
		
	if (index <0){		
		qDebug( "%s %s %d - warning: negative index", __FILE__,  __FUNCTION__, __LINE__ );
		return;
	}
		
	QWidget *newWindow = workspace->windowList()[index];
	if (!newWindow)
		return;
	
	newWindow->show();
	workspace->setActiveWindow ( newWindow );
	workspaceChanged( newWindow );
}

/**
 * \brief called when a window is closed [SLOT]
 * \param o the window which has been deleted from the workspace
 *
 * This slot is called when a window is closed on the workspace.
 * 
 * This slot is connected when a window is added to the
 * workspace.
 *
 */
void qmdiWorkspace::windowDeleted( QObject *o )
{
	int windowNumber = _widgetList.indexOf((QWidget*)(o));

	if (windowNumber == -1)
		return;
		
	tabBar->removeTab( windowNumber );
	_widgetList.removeAt( windowNumber );
	
	if (o == activeWidget)
		activeWidget = NULL;
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
void qmdiWorkspace::on_middleMouse_pressed( int i, QPoint  )
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
void qmdiWorkspace::on_rightMouse_pressed( int i, QPoint p )
{
	showClientMenu( i, p );
}
