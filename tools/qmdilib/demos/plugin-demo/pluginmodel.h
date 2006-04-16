#ifndef __PLUGIN_MODEL_H__
#define __PLUGIN_MODEL_H__

#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QList>

class IPlugin;
class QModelIndex;

class PluginModel : public QStandardItemModel
// 		QAbstractItemModel
{
	Q_OBJECT
public:
	PluginModel( QList<IPlugin*>* p, QObject *parent=0 );
	
/*	QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;	
	QModelIndex parent ( const QModelIndex & index ) const;
	int rowCount ( const QModelIndex & parent = QModelIndex() )const;
	int columnCount ( const QModelIndex & parent = QModelIndex() )const;*/
	
 	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole )const;
// 	bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	Qt::ItemFlags flags ( const QModelIndex & index ) const;
private:
	QList<IPlugin*>	*plugins;
};

#endif// __PLUGIN_MODEL_H__
