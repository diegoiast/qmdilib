#ifndef __QORDERED_MAP_H__
#define __QORDERED_MAP_H__

#include <QString>
#include <QList>
#include <QTextCharFormat>

template <class K, class V>
class QOrderedMapNode
{
public:
	QOrderedMapNode() {}
	QOrderedMapNode( K k, V v ): key(k),value(v)
	{ 
		key = k;
		value = v;
	}
	K key;
	V value;
};


template <class K, class V>
class QOrderedMap
{
public:
	QOrderedMap();
	~QOrderedMap();
	
	void add( K key, V value );
	void remove( K key );
	V* item( K key );
	bool contains( K key );
	void itemNumber( int i );

	int  count();
	void clear();
	bool empty();
	QList< QOrderedMapNode<K,V> > keys();

private:
	QList< QOrderedMapNode<K,V> > nodes;
};


/**
 * \class QOrderedMap
 * \brief The QOrderedMap class is a template class that provides ordered lists.
 * 
 * The QOrderedMap\<T\> is a template based class which provides 
 * a very similar functionality as QMap and QHash. The difference is 
 * that in a QOrderedMap, the items are stored in the order you insert
 * then into the map. While QOrderedMap may be very slow for some operations
 * it will garantee you that you will be able to iterate over the 
 * 
 * - QMap        : stores the items sorted by key
 * - QHash       : stores the items sorted arbitrarily ordered.
 * - QOrderedMap : stores the items sorted by order in which they were inserted to the map
 *  
 * \see QMap
 * \see QHash
 * \see QList
 */


template <class K, class V>
QOrderedMap<K,V>::QOrderedMap()
{
}

template <class K, class V>
QOrderedMap<K,V>::~QOrderedMap()
{
}

/**
 * \brief Add a new key/value pair to the map
 * \param key   - the key name for the value
 * \param value - the value to insert into the map
 * 
 * Inserts a new item with the key \p key and a value of value.
 * If there is already an item with the key \p key, that item's value is replaced with \p value.
 * (the original item is removed).
 */
template <class K, class V>
void QOrderedMap<K,V>::add( K key, V value )
{
	if (contains(key))
		remove( key );
		
	QOrderedMapNode<K, V> node( key, value );
	nodes.append( node );
}

/**
 * \brief Remove a value from the map
 * \param key - the key to be removed from the map
 *
 * Removes the item which key matches the \p key from the map;
 */
template <class K, class V>
void QOrderedMap<K,V>::remove( K key )
{
	int i = 0;
	foreach( K n, nodes )
	{
		i ++;
		if (n.key == key )
		{
			nodes.removeAt( i );
			return;
		}
	}
}

template <class K, class V>
V* QOrderedMap<K,V>::item( K key )
{	
	int c = count();
	for( int i; i<c; i++)
	{
		QOrderedMapNode<K,V> t = nodes[i];
		if (t.key == key)
			return &t.value;
	}

	return NULL;
}

template <class K, class V>
void QOrderedMap<K,V>::itemNumber( int i )
{
	return nodes.items( i );
}

template <class K, class V>
bool QOrderedMap<K,V>::contains( K key )
{
	QOrderedMapNode<K, V> n;

	foreach( n, nodes )
	{
		if (n.key == key )
			return true;
	}
	
	return false;
}

template <class K, class V>
QList< QOrderedMapNode<K,V> > QOrderedMap<K,V>::keys()
{
	return nodes;
}

template <class K, class V>
void QOrderedMap<K,V>::clear()
{
	nodes.clear();
}

template <class K, class V>
int QOrderedMap<K,V>::count()
{
	return nodes.count();
}

template <class K, class V>
bool QOrderedMap<K,V>::empty()
{
	return nodes.empty();
}


#endif //__QORDERED_MAP_H__
