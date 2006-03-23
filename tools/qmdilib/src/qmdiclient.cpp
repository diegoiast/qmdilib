/**
 * \file qmdiclient.cpp
 * \brief Implementation of the qmdi client class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiClient
 */

#include <QObject>
#include "qmdiclient.h"
#include "qmdiserver.h"


/**
 * \class qmdiClient
 * \brief interface for menus and toolbars
 *
 * If you want to use qmdilib, the new widgets which will define
 * their own menus and toolbars must be derived from this class
 * 
 * \code
 * class newclient: public QTextEdit, public qmdiClient
 * {
 *     ...
 * };
 * \endcode
 * 
 * You will have also to insert this mdi client into an mdi server
 * (for example qmdiTabWidget). 
 *
 * Difining menus and toolbars is easy:
 * 
 * \code
 * menus["&File"]->addAction( actionNew );
 * menus["&File"]->addSeparator();
 * menus["&File"]->addAction( actionQuit );
 *
 * toolbars["General"]->addAction( actionNew );
 * \endcode
 * 
 * The menus and toolbars will be merged automatically for you.
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
