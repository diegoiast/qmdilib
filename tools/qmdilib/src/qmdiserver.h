#ifndef __QMDI_SERVER_H__
#define __QMDI_SERVER_H__

/**
 * \file qmdiserver.h
 * \brief Definition of the qmdi server class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL
 * \see qmdiServer
 */

// $Id$ 

#include "actiongroup.h"

class QPoint;

class qmdiClient;
class qmdiHost;

class qmdiServer
{
public:
	qmdiServer();
	virtual ~qmdiServer();
	virtual void		addClient( qmdiClient *client ) = 0;
	virtual void		deleteClient( qmdiClient* );		// see documentation of this method!
	virtual int		getClientsCount() = 0;
	virtual qmdiClient	*getClient( int i ) = 0;
	
	void tryCloseClient( int i );
	void tryCloseAllButClient( int i );
	void tryCloseAllCliens();
	void showClientMenu( int i, QPoint p );
	
// protected:
	qmdiHost *mdiHost;
};

#endif // __QMDI_SERVER_H__
