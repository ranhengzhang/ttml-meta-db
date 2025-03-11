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

QVariant AlbumArtistModel::data(const QModelIndex &index, int role) const {
    if (!artists || !index.isValid() || index.row() >= artists->size())
        return {};

    const QString uuid = artists->at(index.row());
    const Artist &artist = DataBase::artists[uuid];

    if (role == Qt::DisplayRole)
        return artist.getName();

    return {};
}

bool AlbumArtistModel::addData(const QString& uuid) {
    if (DataBase::artists.contains(uuid)) {
        auto &artist = DataBase::artists[uuid];
        if (artist.albums.contains(album_uuid)) {
            return false;
        }
        beginInsertRows(QModelIndex(), artists->size(), artists->size());
        artist.albums.append(album_uuid);
        artists->append(uuid);
        endInsertRows();
        return true;
    }
    return false;
}

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

void AlbumArtistModel::setFamily(QList<QString> *list, const QString& uuid) {
    artists = list;
    album_uuid = uuid;

    refreshAll();
}

void AlbumArtistModel::clean() {
    artists = nullptr;
    album_uuid = {};
    refreshAll();
}

bool AlbumArtistModel::isActive() const {
    return artists != nullptr;
}

void AlbumArtistModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置
    endResetModel();   // 通知视图数据重置完成
}
