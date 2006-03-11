#ifndef __QMDI_CLIENT_H__
#define __QMDI_CLIENT_H__

/**
 * \file qmdiclient.h
 * \brief Definition of the qmdi client class
 * \author Diego Iastrubni (elcuco@kde.org)
 * License LGPL
 * \see qmdiClient
 */

#include "actiongrouplist.h"


class QString;

class qmdiClient
{
public:
	QString name;
	QString fileName;
	qmdiActionGroupList menus;
	qmdiActionGroupList toolbars;
};

#endif // __QMDI_CLIENT_H__
