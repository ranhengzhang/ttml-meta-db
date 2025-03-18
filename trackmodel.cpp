//
// Created by LEGION on 25-3-10.
//

#include "trackmodel.h"

#include <QWidget>

#include "database.h"

TrackModel::TrackModel(QObject *parent): QAbstractListModel(parent) {
    window = dynamic_cast<QWidget *>(parent);
}

int TrackModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return tracks.size();
}

QVariant TrackModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= tracks.size())
        return {};

    const QString uuid = tracks.at(index.row());
    const Track &track = DataBase::tracks[uuid];

    if (role == Qt::DisplayRole)
        return track.getName();

    return {};
}

void TrackModel::setAlbum(const QString &uuid) {
    album_uuid = uuid;
    const auto &album = DataBase::albums[uuid];

    // 将单曲的专辑添加到显示中
    tracks = album.tracks;

    refreshAll();
}

bool TrackModel::addNewData(QString name) {
    // 获取当前专辑
    auto &album = DataBase::albums[album_uuid];

    // 检查专辑下同名单曲
    bool exist = false;
    for (auto &track_uuid: album.tracks) {
        exist |= DataBase::tracks[track_uuid].metas.contains(name);
    }
    if (exist) {
        return false; // 名称已存在，插入失败
    }

    beginInsertRows(QModelIndex(), tracks.size(), tracks.size());
    Track val;

    val.metas.append(name);
    val.albums.append(album_uuid);
    DataBase::tracks.insert(val.getUUID(), val);
    album.tracks.append(val.getUUID());
    tracks.append(val.getUUID());
    endInsertRows();
    return true; // 插入成功
}

bool TrackModel::addOldData(const QString& uuid) {
    if (DataBase::tracks.contains(uuid)) {
        auto &album = DataBase::albums[album_uuid];
        auto &track = DataBase::tracks[uuid];
        if (album.tracks.contains(uuid)) {
            return false;
        }
        beginInsertRows(QModelIndex(), tracks.size(), tracks.size());
        album.tracks.append(uuid);
        track.albums.append(album_uuid);
        tracks.append(uuid);
        endInsertRows();
        return true;
    }
    return false;
}

bool TrackModel::removeTrackFromAlbum(int row) {
    if (row < 0 || row >= tracks.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);

    // 从当前专辑中移除单曲
    const QString track_uuid = tracks.at(row);
    auto &track = DataBase::tracks[track_uuid];
    track.removeFromAlbum(album_uuid);
    endRemoveRows();

    return true;
}

QString TrackModel::getTrackByRow(const int row) const {
    Q_ASSERT(row >= 0 && row < tracks.size());
    return tracks.at(row);
}

void TrackModel::clean() {
    album_uuid = {};
    tracks = {};
    refreshAll();
}

bool TrackModel::isActive() const {
    return !album_uuid.isEmpty();
}

void TrackModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置
    tracks = DataBase::albums.contains(album_uuid) ? DataBase::albums[album_uuid].tracks : QList<QString>{};
    endResetModel();   // 通知视图数据重置完成
}

QString TrackModel::getAlbum() {
    return album_uuid;
}
