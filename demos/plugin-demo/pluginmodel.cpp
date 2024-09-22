/**
 * \file iplugin.cpp
 * \brief Implementation of the PluginModel class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see PluginModel
 */

#include "pluginmodel.h"
#include "iplugin.h"
#include "pluginmanager.h"

/**
 * \class PluginModel
 *
 *
 *
 */
PluginModel::PluginModel(PluginManager *manager, QObject *parent) : QAbstractItemModel(parent) {
    pluginManager = manager;
}

PluginModel::~PluginModel() { pluginManager = nullptr; }

QModelIndex PluginModel::index(int row, int col, const QModelIndex &parent) const {
    return createIndex(row, col, (quintptr) nullptr);

    Q_UNUSED(parent);
}

QModelIndex PluginModel::parent(const QModelIndex &child) const {
    return QModelIndex();
    Q_UNUSED(child);
}

int PluginModel::rowCount(const QModelIndex &parent) const {
    if (pluginManager == nullptr) {
        return 0;
    } else {
        return pluginManager->plugins.count();
    }

    Q_UNUSED(parent);
}

int PluginModel::columnCount(const QModelIndex &) const { return 1; }

QVariant PluginModel::data(const QModelIndex &index, int roles) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= pluginManager->plugins.count()) {
        return QVariant();
    }

    if (roles == Qt::DisplayRole) {
        return pluginManager->plugins[index.row()]->getName();
    } else {
        return QVariant();
    }
}
