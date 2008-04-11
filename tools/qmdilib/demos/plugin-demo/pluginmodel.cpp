/**
 * \file iplugin.cpp
 * \brief Implementation of the PluginModel class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL 2 or 3
 * \see PluginModel
 */

// $Id$

#include "pluginmodel.h"
#include "pluginmanager.h"
#include "iplugin.h"

PluginModel::PluginModel( PluginManager *manager, QObject *parent )
	:QAbstractItemModel(parent)
{
 	pluginManager = manager;
}

PluginModel::~PluginModel()
{
	pluginManager = NULL;
}

QModelIndex PluginModel::index( int row, int col, const QModelIndex &parent ) const
{
	return createIndex( row, col, 0 );
	
	Q_UNUSED( parent );
}

QModelIndex PluginModel::parent( const QModelIndex &child ) const
{
	return QModelIndex();
	Q_UNUSED( child );
}

int PluginModel::rowCount( const QModelIndex &parent ) const
{
	if (pluginManager == NULL)
		return 0;
	else
		return pluginManager->plugins.count();
	
	Q_UNUSED( parent );
}

int PluginModel::columnCount( const QModelIndex &parent ) const
{
	return 1;

	// shut up gcc warnings
	parent.data();
}

QVariant PluginModel::data( const QModelIndex &index, int roles ) const
{	
	if (!index.isValid())
		return QVariant();

	if (index.row() >= pluginManager->plugins.count())
		return QVariant();

	if (roles == Qt::DisplayRole)
		return pluginManager->plugins[index.row()]->getName();
	else
		return QVariant();
}
