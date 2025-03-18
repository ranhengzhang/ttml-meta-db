//
// Created by LEGION on 25-3-10.
//

#include "AlbumArtistModel.h"

#include <QWidget>

#include "database.h"

AlbumArtistModel::AlbumArtistModel(QObject *parent) {
    view = dynamic_cast<QWidget *>(parent);
}

int AlbumArtistModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return artists ? artists->size() : 0;
}

QVariant AlbumArtistModel::data(const QModelIndex &index, const int role) const {
    if (!artists || !index.isValid() || index.row() >= artists->size())
        return {};

    const QString uuid = artists->at(index.row());
    const Artist &artist = DataBase::artists[uuid];

    if (role == Qt::DisplayRole)
        return artist.getName();

    return {};
}

/**
 * 添加当前专辑到歌手
 * @param artist_uuid 目标歌手的 UUID
 * @return 添加成功/失败
 */
bool AlbumArtistModel::addData(const QString& artist_uuid) {
    if (DataBase::artists.contains(artist_uuid)) {
        auto &artist = DataBase::artists[artist_uuid];
        if (artist.albums.contains(album_uuid)) {
            return false;
        }

        beginInsertRows(QModelIndex(), artists->size(), artists->size());
        artist.albums.append(album_uuid);
        artists->append(artist_uuid);
        endInsertRows();
        return true;
    }
    return false;
}

/**
 * 从选中歌手中删除专辑
 * @param row 选中歌手的行号
 * @return 删除成功/失败
 */
bool AlbumArtistModel::removeData(const int row) {
    if (row < 0 || row >= artists->size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);
    // 从选中歌手中移除专辑
    const QString artist_uuid = artists->at(row);
    auto &artist = DataBase::artists[artist_uuid];
    artist.albums.removeAll(album_uuid);
    DataBase::albums[album_uuid].removeFromArtist(artist_uuid);
    endRemoveRows();

    return true;
}

/**
 * 设置当前专辑的 UUID 和歌手列表
 * @param album_artist_list 当前专辑的歌手列表
 * @param uuid 当前专辑的 UUID
 */
void AlbumArtistModel::setFamily(QList<QString> *album_artist_list, const QString& uuid) {
    beginResetModel();
    artists = album_artist_list;
    album_uuid = uuid;
    endResetModel();
}

/**
 * 清除当前专辑信息
 */
void AlbumArtistModel::clean() {
    beginResetModel();
    artists = nullptr;
    album_uuid = {};
    endResetModel();
}

/**
 * 当前 Model 是否有数据
 * @return 有/无数据
 */
bool AlbumArtistModel::isActive() const {
    return artists != nullptr;
}
