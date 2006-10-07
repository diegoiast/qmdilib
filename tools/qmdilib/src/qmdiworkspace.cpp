/**
 * \file qmdiworkspace.cpp
 * \brief implementation of the qmdi Workspace
 * \author Diego Iastrubni <elcuco@kde.org>
 * Licence LGPL
 * \see qmdiWorkspace
 */
 
// $Id$

#include <QWidget>
#include <QTabBar>
#include <QWorkspace>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMenu>

#include "qmdiclient.h"
#include "qmdihost.h"
#include "qmdiworkspace.h"
#include "qmditabbar.h"

/**
 * \class qmdiWorkspace
 * \brief An advanced work space-widget, which is capable of changing menus and toolbars on the fly
 * 
 * This class is a new mdi server, based on top of QWorkspace. It is built
 * with a similar API to qmdiTabWidget and QTabWidget. Since the API is similar
 * this means you can switch applications from QTabWidget to qmdiWorkspace in
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
 * widget is removed from the qmdiHost, and only then the new mdi client is added
 * to the qmdiHost.
 *
 * To use this class properly, insert it into a QMainWindow which also derives qmdiHost,
 * and insert into it QWidgets which also derive qmdiClient.
 *
 * \see qmdiTabBar
 */


/**
 * \brief default constructor
 * \param parent the parent of this widget
 * \param host the mdi host to connect to
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
		
	tabBar = new qmdiTabBar;
	connect( tabBar, SIGNAL(middleMousePressed(int,QPoint)), this, SLOT(tryCloseClient(int)));
	connect( tabBar, SIGNAL(rightMousePressed(int,QPoint)), this, SLOT(showClientMenu(int,QPoint)));
	
	workspace = new QWorkspace;	
	mainLayout = new QVBoxLayout;
	headerLayout = new QHBoxLayout;
	headerLayout->setMargin(0);
	headerLayout->setSpacing(0);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	
	tabBar->setDrawBase( false );
	headerLayout->addWidget( tabBar );
	mainLayout->addItem(headerLayout);
	mainLayout->addWidget(workspace);
	
	setLayout( mainLayout );
	connect( workspace, SIGNAL(windowActivated(QWidget*)), this, SLOT(workspaceChanged(QWidget*)));
	connect( tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabBarChanged(int)));
}

/**
 * \brief add a new mdiclient to the workspace
 * \param client the client to be added to the workspace
 *
 * Adds a mdi client to the mdi server. Internally calls
 * addTab()
 *
 * \see addTab
 */
void qmdiWorkspace::addClient( qmdiClient *client )
{
	QWidget *w = dynamic_cast<QWidget*>(client);

	if (w == NULL)
		return;		
	
	addTab( w, client->name );
}

/**
 * \brief add a new mdiclient to the workspace
 * \param widget the widget to be added to the workspace
 * \param name the name to be assosiated with this widget
 *
 * This function does the job of adding an mdi client to the mdi
 * server. It will do the job behind the scene of setting up
 * the mdi client, and reparenting it as needed. The name is
 * generally the name you will see on the tabbar.
 *
 * Since this class is API compatible to QTabWidget,
 * is takes the same arguments.
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
 * Set the widget to be put on onf of the corners of this widget.
 * Usually used to put "close" or "new" buttons on the right or left
 * of the widget.
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
// TODO only tabs on the top is fully supported
		case Qt::BottomLeftCorner: 
			cornerWidget1 = widget;
			break;
		case Qt::BottomRightCorner:
			cornerWidget2  = widget;
			break;
	}
	
	// now lets  remove header layout
	// and reconstruct it, and finally add it back to the main layout
	// TODO: is there a smarter way of doing this?
	mainLayout->removeItem( headerLayout );
	mainLayout->removeWidget( workspace );
	
	delete headerLayout;
	headerLayout = new QHBoxLayout;
	headerLayout->setMargin(0);
	headerLayout->setSpacing(0);
	
	if (cornerWidget1) 
		headerLayout->addWidget(cornerWidget1);
	headerLayout->addWidget(tabBar);
	if (cornerWidget2) 
		headerLayout->addWidget(cornerWidget2);
	
	mainLayout->addItem(headerLayout);
	mainLayout->addWidget(workspace);
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
		
	return workspace->windowList().at( i );
}

/**
 * \brief return the number of widgets available on the workspace
 *
 * Returns the number of widgets (or windows) aviable. The number
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
 * \brief called when the active window in the workspace is changed [SLOT]
 * \param w the new window which has been focused
 *
 * This slot is called when a new window is selected on the workspace.
 * This slot will also selecte the new tab on the tab header as well as
 * merge the mdi client menus and toolbars.
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

	// try to update the mdi host
	QMainWindow *m = dynamic_cast<QMainWindow*>(mdiHost);

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
void qmdiWorkspace::tabBarChanged( int index )
{
	if (!workspace)
			return;
	
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
 * \brief request an mdi client to close
 * \param i the number of the client (tab) to be closed
 *
 * Call this slot to ask the mdi client to close itself.
 * The mdi client may show a dialog to ask for saving. It's not
 * ganranteed that the action will be handled as the mdi client
 * can abort the action.
 *
 * \see qmdiClient::closeClient()
 */
void qmdiWorkspace::tryCloseClient( int i )
{
	qmdiClient *client = dynamic_cast<qmdiClient*>(widget(i));

	// try to close only mdi clients
	// TODO should be close also non mdi clients...?
	if (!client)
		return;

	client->closeClient();
}

/**
 * \brief request to close all other clients
 * \param i the number of the client to keep open
 *
 * Call this slot to ask all the mdi clients (but the widget found at
 * index \b i in the tab widget, passed as a parameter).
 * Each mdi client may show a dialog to ask for saving. It's not
 * guaranteed that the action will be handled as the mdi client
 * can abort the action. At the end, only the client number i will
 * not be asked to close itself.
 *
 * If some widget on the mdi server does not derive (implements) 
 * the qmdiClient interface, the widget will not be closed.
 *
 * \see qmdiClient::closeClient() tryCloseClient() tryCloseAllCliens
 */
void qmdiWorkspace::tryCloseAllButClient( int i )
{
	int c = count();
	QWidget *w = widget(i);

	for( int j=0; j<c; j++ )
	{
		QWidget *w2 = widget(j);
		if (w == w2)
			continue;
		
		qmdiClient *client = dynamic_cast<qmdiClient*>(w2);
		if (!client)
			continue;

		client->closeClient();
	}
}

/**
 * \brief try to close all mdi clients
 *
 * Call this slot when you want to close all the mdi clients.
 */
void qmdiWorkspace::tryCloseAllCliens()
{
	int c = count();

	for( int i=0; i<c; i++ )
	{
		qmdiClient *client = dynamic_cast<qmdiClient*>(widget(i));
		if (!client)
			continue;

		client->closeClient();
	}	
}

/**
 * \brief display the menu of a specific mdi client
 * \param i the mouse button that has been pressed
 * \param p the location of the mouse click
 *
 * This function is called when a user presses the left mouse
 * button on the tab bar of the tab widget. The coordinates of the
 * click are passed on the parameter \b p , while the
 * mouse button which has been pressed is passed on the
 * parameter \b p .
 *
 * This slot is connected to rightMousePressed signal of the qmdiTabBar
 * at the constructor of this class.
 * 
 * \see qmdiTabBar
 */
void qmdiWorkspace::showClientMenu( int i, QPoint p )
{
	QAction *closeThis	= new QAction(tr("Close this window"), this);
	QAction *closeOthers	= new QAction(tr("Close other windows"), this);
	QAction *closeAll	= new QAction(tr("Close all windows"), this);
	QMenu *menu = new QMenu( tr("Local actions") );
	menu->addAction( closeThis );
	menu->addAction( closeOthers );
	menu->addAction( closeAll );

	QAction *q = menu->exec( this->mapToGlobal(p) );

	if ( q == closeThis)
	{
		tryCloseClient( i );
	}
	else if  (q == closeOthers)
	{
		tryCloseAllButClient( i );
	}
	else if (q == closeAll )
	{
		tryCloseAllCliens();
	}
}
