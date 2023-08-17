/*
 * elementtreemodel.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#include "elementtreemodel.h"
ElementTreeModel::ElementTreeModel(QObject *parent)
    : QAbstractItemModel(parent), rootItem(nullptr)
{
}

void ElementTreeModel::setData(const ViewerAutoObject *data)
{
    beginResetModel();
    rootItem = data;
    endResetModel();
}

QVariant ElementTreeModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid() || !rootItem)
       return QVariant();

   if (role == Qt::DisplayRole)
   {
       const AutoObject *item = static_cast<const ViewerAutoObject*>(index.internalPointer());
       if (!index.parent().isValid()) // Top-level items
       {
           if (index.column() == 0)
           {
               string treeItem = (!item->ClassName().empty()) ? item->ClassName() : (!item->AccName().empty()) ? item->AccName() : item->ObjName();
               return QString::fromStdString(treeItem);
           }

       }
       else // Child items
       {
           if (index.column() == 0)
           {
               const AutoObject *parentItem = static_cast<const AutoObject*>(index.parent().internalPointer());
               if (parentItem && index.row() < parentItem->Children().size())
               {
                   AutoObject* childObj = parentItem->Children()[index.row()];
                   string treeItem = (!childObj->ClassName().empty()) ? childObj->ClassName() :
                                                                        (!childObj->AccName().empty()) ? childObj->AccName() :
                                                                                                         childObj->ObjName();
                   return QString::fromStdString(treeItem);
               }
           }
       }
   }
   return QVariant();
}

Qt::ItemFlags ElementTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || !rootItem)
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ElementTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == 0)
            return tr("Name");
    }
    return QVariant();
}

QModelIndex ElementTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!rootItem || row < 0 || column < 0)
        return QModelIndex();

    if (!parent.isValid()) // Top-level items
    {
        if (row < rootItem->Children().size() && column == 0)
            return createIndex(row, column, (void*)rootItem);
    }
    else // Child items
    {
        const AutoObject *parentItem = static_cast<const AutoObject*>(parent.internalPointer());
        if (row < parentItem->Children().size() && column == 0)
            return createIndex(row, column, parentItem->Children()[row]);
    }
    return QModelIndex();
}

QModelIndex ElementTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid() || !rootItem)
            return QModelIndex();

        if (!index.internalPointer()) // Invalid internalPointer, return the root item
            return QModelIndex();

        const AutoObject *childItem = static_cast<const AutoObject*>(index.internalPointer());
        AutoObject *parentItem = childItem->mParent;
        if (!parentItem) // If no parent or the parent is the root, return QModelIndex
            return QModelIndex();

        // Find the row of the parent item in its parent's children vector
        int row = -1;
        for (size_t i = 0; i < parentItem->Children().size(); ++i)
        {
            if (parentItem->Children()[i] == childItem)
            {
                row = static_cast<int>(i);
                break;
            }
        }

        if (row == -1)
            return QModelIndex(); // Item not found in the parent's children vector

        return createIndex(row, 0, parentItem);
}

int ElementTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!rootItem)
        return 0;

    if (!parent.isValid()) // Top-level items
        return 1;//static_cast<int>(rootItem->vChildrenObj.size());

    const ViewerAutoObject *parentItem = static_cast<const ViewerAutoObject*>(parent.internalPointer());
    return static_cast<int>(parentItem->Children().size());
}

int ElementTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}
