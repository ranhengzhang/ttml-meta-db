//
// Created by LEGION on 25-8-1.
//

#include "extramodel.h"

ExtraModel::ExtraModel(QObject *parent) {
    view = dynamic_cast<QTableView *>(parent);
}

int ExtraModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return extras == nullptr ? 0 : extras->size();
}

int ExtraModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

QVariant ExtraModel::data(const QModelIndex &index, int role) const {
    if (extras == nullptr || !index.isValid())
        return {};

    // 明确阻止检查状态显示
    if (role == Qt::CheckStateRole)
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return extras->at(index.row())[index.column()];
    return {};
}

bool ExtraModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    (*extras)[index.row()][index.column()] = value.toString();
    emit dataChanged(index, index);
    return true;
}

bool ExtraModel::addData(const QString &key, const QString &value) {
    if (extras->contains({key, value}) && key.isEmpty() && value.isEmpty()) return false;

    beginInsertRows(QModelIndex(), extras->size(), extras->size());
    const QList val{key, value};

    extras->append(val);
    endInsertRows();
    return true; // 插入成功
}

Qt::ItemFlags ExtraModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == 0 || index.column() == 1) {
        flags |= Qt::ItemIsEditable;
        flags &= ~Qt::ItemIsUserCheckable; // 强制移除复选框标志
    }
    return flags;
}

bool ExtraModel::insertRows(int row, int count, const QModelIndex &parent) {
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        extras->insert(row, {"", ""}); // 默认值
    }
    endInsertRows();
    return true;
}

bool ExtraModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        extras->removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool ExtraModel::isActive() const {
    return extras != nullptr;
}

void ExtraModel::setFamily(QList<QList<QString> > *meta_list, const QString &track_uuid) {
    beginResetModel();
    extras = meta_list;
    this->track_uuid = track_uuid;
    endResetModel();
}

void ExtraModel::clean() {
    beginResetModel();
    extras = {};
    track_uuid = {};
    endResetModel();
}

QVariant ExtraModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Key";
            case 1: return "Value";
            default: return {};
        }
    }
    return {};
}
