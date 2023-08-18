/*
 * elementtreemodel.h
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#ifndef ELEMENTTREEMODEL_H
#define ELEMENTTREEMODEL_H
#include <QAbstractItemModel>
#include <QTreeView>
#include "viewerautoobject.h"

class ElementTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ElementTreeModel(QObject *parent = nullptr);
    void setData(const ViewerAutoObject *data);

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;


private:
    const AutoObject *rootItem;
};

#endif // ELEMENTTREEMODEL_H
