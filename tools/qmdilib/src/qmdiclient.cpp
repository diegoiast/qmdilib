/**
 * \file qmdiclient.cpp
 * \brief Implementation of the qmdi client class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiClient
 */
 
// $Id$ 

#include <QObject>
#include "qmdiclient.h"
#include "qmdiserver.h"


/**
 * \class qmdiClient
 * \brief Interface for menus and toolbars
 *
 * If you want to use qmdilib, every one of your widgets must inherit
 * qmdiClient and define their own menus using this class. Any widget
 * which will be inserted into a qmdiServer and does not inherit qmdiClient
 * will not show special menus and toolbars.
 * 
 * On the documentation of qmdilib, any widget inserted into a qmdiServer
 * that inherits qmdiClient will be called \b mdi \b client. In the following 
 * example the class \b newclient is a \b mdi \b client.
 * 
 * \code
 * class newclient: public QTextEdit, public qmdiClient
 * {
 *     ...
 * };
 * \endcode
 * 
 * Defining menus and toolbars is easy:
 * 
 * \code
 * newclient::newclient()
 * {
 * 	// some code ...
 * 
 *	menus["&File"]->addAction( actionNew );
 *	menus["&File"]->addSeparator();
 *	menus["&File"]->addAction( actionQuit );
 * 
 * 	toolbars["General"]->addAction( actionNew );
 * }
 *
 * \endcode
 * 
 * You will have also to insert this mdi client into an mdi server
 * (for example qmdiTabWidget). The menus and toolbars will be merged 
 * automatically for you every time you widget is selected on the mdi server.
 * 
 * \see qmdiActionGroupList
 * \see qmdiTabWidget
 */

qmdiClient::qmdiClient()
{
	mdiServer = NULL;
}

qmdiClient::~qmdiClient()
{
	if ((mdiServer != NULL) && (myself != NULL))
	{
		mdiServer->clientDeleted( myself );

//		this code is way cooler, but does not work.
//		the dynamic_cast always returns NULL
//		this is the reason for duplicating "this" with "myself"
//		qmdiClient *i = this;
//		mdiServer->clientDeleted( dynamic_cast<QObject*>(i) );
	}

	mdiServer = NULL;
	myself    = NULL;
}

/**
 * \var qmdiClient::name
 * \brief The name of the mdi client
 * 
 * This property defines the name of the mdi client. The \b name 
 * is used by qmdiTabWidget for setting the tab name, when inserting 
 * the widget into the tab widget.
 */


/**
 * \var qmdiClient::fileName
 * \brief The file opened by this mdi client
 * 
 * This property defines the file name which is opened by the mdi client.
 * The file name is not really used inside \b qmdilib , but it's left 
 * for usage outside of library. The \b name property should reflect
 * this property, but it's not really needed.
 */

/**
 * \var qmdiClient::menus
 * \brief The list of menus defined by this mdi client
 * 
 * The list of menus this mdi client defines, and will be merged
 * each time the mdi client is showed on screen. 
 * 
 * The syntax for defining menus is:
 * 
 * \code
 * menus->addAction( actionNew );
 * menus->addAction( actionOpen );
 * menus->addAction( actionSave );
 * menus->addSeparator();
 * menus->addAction( actionQuit );
 * \endcode
 * 
 * \see qmdiClient::toolbars
 */

/**
 * \var qmdiClient::toolbars
 * \brief The list of toolbars defined by this mdi client
 * 
 * The list of toolbars this mdi client defines, and will be merged
 * each time the mdi client is showed on screen.
 * 
 * The syntax for defining toolbars is:
 * 
 * \code
 * toolbars->addAction( actionNew );
 * toolbars->addSeparator();
 * toolbars->addAction( actionOpen );
 * toolbars->addAction( actionSave );
 * \endcode
 * 
 * \see qmdiClient::menus
 */


/**
 * \var qmdiClient::mdiServer
 * \brief A pointer to the mdi server in which this client is inserted.
 * 
 * When the mdi client is inserted into a mdi server, this property
 * is set to point to the correponding mdi server. This property is
 * read only, and you should not assign value ot it.
 * 
 * You can use this property to insert new clients into the server,
 * from this client.
 * 
 * \see qmdiServer
 */

/**
 * \var qmdiClient::myself
 * \brief A cast of this mdi client into a QObject
 * 
 * This property contains a cast of this client, into a 
 * QObject (if the instanse inherits QObject). 
 * 
 * This is a read only property, which is set by the mdi severs
 * when this client is inserted into the server.
 *
 * \see qmdiTabWidget::tabInserted ( int index )
 */

/**
 * \brief close the mdi client
 * \return true if the widget is closed after this call
 * 
 * If you want to delete mdi clients from your mdi server, sometimes
 * just deleting this client is not the smart move. For example if the
 * client contains an user editable document, the user should be asked
 * to save his changes, and maybe even abort.
 *
 * By default this function does:
 *  - calls canCloseClient to find out if it's ok to closeClient()
 *  - if the function returns true, it will try to cast the mdi client to a
 *    QObject, calls deleteLater() and returns true and the object 
 *    will be deleted afterwards.
 *  - if cast failed or canCloseClient() returned false, a negative value
 *    is returned, and the object is not destructed.
 * 
 * This means that for read only clients you can leave the default. On R/W
 * clients which derive QObject, you will have to override canCloseClient().
 * If your derived class does not derive QObject, you will need to overide this
 * function as well.
 *
 * \see canCloseClient()
 * \see QObject::deleteLater()
 */
bool qmdiClient::closeClient()
{
	if (canCloseClient())
	{
		if (myself != NULL)
		{
			myself->deleteLater();
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

/**
 * \brief check if the mdi client is valid for closing
 * \return true is it is ok to destruct the mdi client
 *
 * This function should return true if the mdi client is safe for
 * removal from the mdi client. Clients are valid for removal usually
 * if no changes have been made, or the user accepted to save then.
 *
 * You can define your own logic by re-implementing this function
 * in your derived classes.
 * 
 * \see closeClient()
 */
bool qmdiClient::canCloseClient()
{
	return true;
}
