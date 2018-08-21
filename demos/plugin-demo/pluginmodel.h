#ifndef __PLUGINMODEL_H__
#define __PLUGINMODEL_H__

/**
 * \file pluginmodel.h
 * \brief Definition of the PluginModel class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL 2 or 3
 * \see PluginModel
 */

// $Id$

#include <QAbstractItemModel>

class PluginManager;

class PluginModel : public QAbstractItemModel
{
	Q_OBJECT
public:

	PluginModel( PluginManager *manager, QObject *parent = 0 );
	~PluginModel();
 
	QModelIndex index( int row, int col, const QModelIndex &parent = QModelIndex() ) const;
	QModelIndex parent( const QModelIndex & child ) const;
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
	
private:
	PluginManager *pluginManager;
};
 
#endif // #ifndef __PLUGINMODEL_H__
