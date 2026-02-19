//
// Created by LEGION on 25-3-8.
//

#include "artistmodel.h"

#include <QMessageBox>

#include "../database/types/artist.h"
#include "../database/database.h"

ArtistModel::ArtistModel(QObject *parent) : QAbstractListModel(parent) {
    _view = dynamic_cast<QWidget *>(parent);
}

int ArtistModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return _artists.size();
}

QVariant ArtistModel::data(const QModelIndex &index, const int role) const {
    if (!index.isValid() || index.row() >= _artists.size())
        return {};

    const QString uuid = _artists.at(index.row());
    const Artist &artist = DataBase::artists[uuid];

    if (role == Qt::DisplayRole)
        return artist.getName();

    return {};
}

Qt::ItemFlags ArtistModel::flags(const QModelIndex &index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

/**
 * 添加一个新的歌手
 * @param name 歌手名
 * @return 添加成功/失败
 */
bool ArtistModel::addData(const QString &name) {
    bool exist = false;
    for (auto &artist: DataBase::artists) {
        exist |= artist.metas.contains(name);
    }
    if (exist) {
        return false; // 名称已存在，插入失败
    }

    int index = 0;
    for (index = 0; index < _artists.size(); ++index) {
        if (DataBase::artists[_artists[index]].getName() > name)
            break;
    }

    beginInsertRows(QModelIndex(), index, index);

    Artist val;
    val.metas.append(name);

    DataBase::artists.insert(val.getUUID(), val);
    _artists.insert(index, val.getUUID());
    endInsertRows();
    return true; // 插入成功
}

/**
 * 删除歌手并从专辑歌手中清除
 * @param row 选中歌手在 model 中的位置
 * @return 删除成功/失败
 */
bool ArtistModel::removeArtist(const int row) {
    if (row < 0 || row >= _artists.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);
    const QString artist_uuid = _artists.at(row);
    _artists.remove(row);
    auto &artist = DataBase::artists[artist_uuid];
    artist.remove();
    endRemoveRows();
    return true;
}

/**
 * 获取 model 中选中的歌手 UUID
 * @param row 选中歌手在 model 中的行号
 * @return 歌手的 UUID
 */
QString ArtistModel::getArtistByRow(const int row) const {
    Q_ASSERT(row >= 0 && row < _artists.size());
    return _artists.at(row);
}

void ArtistModel::refreshAll() {
    beginResetModel();

    _artists = DataBase::artists.keys();

    std::sort(_artists.begin(), _artists.end(), [](const QString &keyA, const QString &keyB) {
        return DataBase::artists[keyA].getName() < DataBase::artists[keyB].getName();
    });

    endResetModel();
}
