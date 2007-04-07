#ifndef __QMDI_SERVER_H__
#define __QMDI_SERVER_H__

/**
 * \file qmdiserver.h
 * \brief Definition of the qmdi server class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiServer
 */
 
// $Id$ 

#include "actiongroup.h"

class qmdiClient;
class qmdiHost;

class qmdiServer
{
public:
	virtual ~qmdiServer();
	virtual void addClient( qmdiClient *client ) = 0;
	virtual void deleteClient( qmdiClient* );
// protected:
	qmdiHost *mdiHost;
};

#endif // __QMDI_SERVER_H__
