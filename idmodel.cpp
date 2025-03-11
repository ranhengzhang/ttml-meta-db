//
// Created by LEGION on 25-3-10.
//

#include "idmodel.h"

#include <QWidget>

QList<QString> IDModel::id_options = {
    "ncmMusicId",
    "qqMusicId",
    "spotifyId",
    "appleMusicId",
    "isrc"
};

IDModel::IDModel(QObject *parent) {
    view = dynamic_cast<QWidget*>(parent);
}

int IDModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return ids == nullptr ? 0 : ids->size();
}

int IDModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

QVariant IDModel::data(const QModelIndex &index, int role) const {
    if (ids == nullptr || !index.isValid())
        return {};

    // 明确阻止检查状态显示
    if (role == Qt::CheckStateRole)
        return {};

    if (role == Qt::DisplayRole || role ==  Qt::EditRole)
        return ids->at(index.row())[index.column()];
    return {};
}

bool IDModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (index.column() == 0) {
        if (!id_options.contains(value.toString()))
            return false;
        (*ids)[index.row()][0] = value.toString();
    } else {
        (*ids)[index.row()][1] = value.toString();
    }
    emit dataChanged(index, index);
    return true;
}

bool IDModel::addData(QString key, QString value) {
    // bool exist = false;
    // for (auto &item: *ids) {
    //     exist |= item[0] == key && item[1] == value;
    // }
    // if (exist) {
    //     return false;
    // }
    if (ids->contains({key, value})) return false;

    beginInsertRows(QModelIndex(), ids->size(), ids->size());
    QList val{key, value};

    ids->append(val);
    endInsertRows();
    return true; // 插入成功
}

Qt::ItemFlags IDModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == 0 || index.column() == 1) {
        flags |= Qt::ItemIsEditable;
        flags &= ~Qt::ItemIsUserCheckable; // 强制移除复选框标志
    }
    return flags;
}

bool IDModel::insertRows(int row, int count, const QModelIndex &parent) {
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        ids->insert(row, {id_options.first(), ""}); // 默认值
    }
    endInsertRows();
    return true;
}

bool IDModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        ids->removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool IDModel::isActive() {
    return ids != nullptr;
}

void IDModel::setFamily(QList<QList<QString>> *list, const QString &uuid) {
    ids = list;
    track_uuid = uuid;

    refreshAll();
}

void IDModel::clean() {
    ids = {};
    track_uuid = {};

    refreshAll();
}

QStringList IDModel::options() {
    return id_options;
}

QVariant IDModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Key";
            case 1: return "Value";
        }
    }
    return QVariant();
}

void IDModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置
    endResetModel();   // 通知视图数据重置完成
}
