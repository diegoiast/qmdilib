#include <QModelIndex>

#include "iplugin.h"
#include "pluginmodel.h"

PluginModel::PluginModel( QList<IPlugin*>* p, QObject *parent )
// 	: QAbstractItemModel( parent )
	: QStandardItemModel( parent )
{
	plugins = p;
}
/*
QModelIndex PluginModel::index ( int row, int column, const QModelIndex &parent ) const
{
}

QModelIndex PluginModel::parent ( const QModelIndex &index ) const
{
}

int PluginModel::rowCount ( const QModelIndex &parent ) const
{
	return plugins.count();
}

int PluginModel::columnCount ( const QModelIndex &parent ) const
{
	return 2;
}
*/


QVariant PluginModel::data ( const QModelIndex &index, int role ) const
{
#if 0	
	IPlugin *plugin = NULL;

	if (plugins)
		plugin = (*plugins)[index.row()];
	else
		plugin = NULL;
	
	
	if (plugin)
	{
		qDebug( "getting data for row: %d, column: %d [%s]", index.row(), index.column(), qPrintable(plugin->getName()) );
		return QVariant(plugin->getName());
	}
	else
	{
		qDebug( "getting data for row: %d, column: %d", index.row(), index.column() );
		return "no name";
	}
#else
	return QStandardItemModel::data(index,role);
#endif
}

Qt::ItemFlags PluginModel::flags ( const QModelIndex & index ) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
