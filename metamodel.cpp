//
// Created by LEGION on 25-3-8.
//

#include "metamodel.h"

#include <QMessageBox>

MetaModel::MetaModel(QObject *parent): QAbstractListModel(parent) {
    view = dynamic_cast<QWidget *>(parent);
}

void MetaModel::setEntity(DataEntity *node) {
    beginResetModel();
    entity = node;
    endResetModel();
}

int MetaModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return entity ? entity->metas.size() : 0;
}

QVariant MetaModel::data(const QModelIndex &index, const int role) const {
    if (!entity || !index.isValid() || index.row() >= entity->metas.size())
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return entity->metas.at(index.row());

    return {};
}

Qt::ItemFlags MetaModel::flags(const QModelIndex &index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

/**
 * 添加一个名称
 * @param name 添加的名称
 * @return 添加成功/失败
 */
bool MetaModel::addData(const QString &name) {
    if (entity->metas.contains(name)) {
        return false;
    }

    beginInsertRows(QModelIndex(), entity->metas.size(), entity->metas.size());

    entity->metas.append(name);
    endInsertRows();
    return true; // 插入成功
}

bool MetaModel::setData(const QModelIndex &index, const QVariant &value, const int role) {
    const auto meta = entity->metas.at(index.row());
    if (value == meta) return true;

    // 查重
    if (entity->metas.contains(value.toString())) {
        QMessageBox::critical(view, "失败", "重复的歌手名");
    }

    // 清除
    if (value.toString().isEmpty()) {
        return removeMeta(index.row());
    }

    // 覆盖
    if (role == Qt::EditRole) {
        entity->metas.replace(index.row(), value.toString());
        emit dataChanged(index, index); // 通知视图刷新
        return true;
    }
    return false;
}

bool MetaModel::removeMeta(const int row) {
    if (row < 0 || row >= entity->metas.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);
    entity->metas.removeAt(row);
    endRemoveRows();
    return true;
}

void MetaModel::clean() {
    beginResetModel();
    entity = nullptr;
    endResetModel();
}

bool MetaModel::isActive() const {
    return entity != nullptr;
}
