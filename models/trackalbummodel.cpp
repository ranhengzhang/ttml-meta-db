//
// Created by LEGION on 25-3-10.
//

#include "trackalbummodel.h"

#include <QWidget>

#include "../database/database.h"

TrackAlbumModel::TrackAlbumModel(QObject *parent) {
    view = dynamic_cast<QWidget *>(parent);
}

int TrackAlbumModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return albums ? albums->size() : 0;
}

QVariant TrackAlbumModel::data(const QModelIndex &index, int role) const {
    if (!albums || !index.isValid() || index.row() >= albums->size())
        return {};

    const QString uuid = albums->at(index.row());
    const Album &album = DataBase::albums[uuid];

    if (role == Qt::DisplayRole)
        return album.getName();

    return {};
}

bool TrackAlbumModel::addData(const QString &uuid) {
    if (DataBase::albums.keys().contains(uuid)) {
        auto &album = DataBase::albums[uuid];
        if (album.tracks.contains(track_uuid)) {
            return false;
        }
        beginInsertRows(QModelIndex(), albums->size(), albums->size());
        album.tracks.append(track_uuid);
        albums->append(uuid);
        endInsertRows();
        return true;
    }
    return false;
}

bool TrackAlbumModel::removeData(int row) {
    if (row < 0 || row >= albums->size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);

    // 从选中歌手中移除专辑
    const QString album_uuid = albums->at(row);
    auto &album = DataBase::albums[album_uuid];
    album.tracks.removeAll(track_uuid);
    DataBase::tracks[track_uuid].removeFromAlbum(album_uuid);
    endRemoveRows();

    return true;
}

/**
 * 设置单曲所有专辑以及单曲 UUID
 * @param album_list 专辑列表
 * @param track_uuid 单曲 UUID
 */
void TrackAlbumModel::setFamily(QList<QString> *album_list, const QString &track_uuid) {
    beginResetModel();
    albums = album_list;
    this->track_uuid = track_uuid;
    endResetModel();
}

void TrackAlbumModel::clean() {
    beginResetModel();
    albums = nullptr;
    track_uuid = {};
    endResetModel();
}

bool TrackAlbumModel::isActive() const {
    return albums != nullptr;
}
