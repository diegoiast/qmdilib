#ifndef __QMDI_CLIENT_H__
#define __QMDI_CLIENT_H__

/**
 * \file qmdiclient.h
 * \brief Definition of the qmdi client class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL
 * \see qmdiClient
 */
 
 // $Id$

#include "actiongrouplist.h"

class QString;
class qmdiServer;
class qmdiHost;

class qmdiClient
{
public:
	qmdiClient( QString newName="" );
	virtual ~qmdiClient();

	virtual bool closeClient();
	virtual bool canCloseClient();
	virtual QString mdiClientFileName();
	
	QString getMDIClientName();
	void setMDIclientName( QString newName );
	qmdiServer* getMDIServer();
	void setMDIServer( qmdiServer* newServer );
	
	qmdiActionGroupList	menus;
	qmdiActionGroupList	toolbars;
protected:
	qmdiServer*		mdiServer;
	QString			name;

friend class qmdiHost;

};

#endif // __QMDI_CLIENT_H__
