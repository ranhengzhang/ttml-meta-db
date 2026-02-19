//
// Created by LEGION on 25-3-10.
//

#include "trackmodel.h"

#include <QWidget>

#include "../database/database.h"

TrackModel::TrackModel(QObject *parent) : QAbstractListModel(parent) {
    _window = dynamic_cast<QWidget *>(parent);
}

int TrackModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return _tracks.size();
}

QVariant TrackModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= _tracks.size())
        return {};

    const QString uuid = _tracks.at(index.row());
    const Track &track = DataBase::tracks[uuid];

    if (role == Qt::DisplayRole)
        return track.getName();

    return {};
}

/**
 * 设置当前专辑
 * @param uuid 专辑 UUID
 */
void TrackModel::setAlbum(const QString &uuid) {
    _album_uuid = uuid;
    const auto &album = DataBase::albums[uuid];

    // 将单曲的专辑添加到显示中
    _tracks = album.tracks;

    refreshAll();
}

bool TrackModel::addNewData(const QString &name) {
    // 获取当前专辑
    auto &album = DataBase::albums[_album_uuid];

    // 检查专辑下同名单曲
    bool exist = false;
    for (auto &track_uuid: album.tracks) {
        exist |= DataBase::tracks[track_uuid].getName() == name;
    }
    if (exist) {
        return false; // 名称已存在，插入失败
    }

    beginInsertRows(QModelIndex(), _tracks.size(), _tracks.size());
    Track val;

    val.metas.append(name);
    val.albums.append(_album_uuid);
    DataBase::tracks.insert(val.getUUID(), val);
    album.tracks.append(val.getUUID());
    _tracks.append(val.getUUID());
    endInsertRows();
    return true; // 插入成功
}

bool TrackModel::addOldData(const QString &uuid) {
    if (DataBase::tracks.keys().contains(uuid)) {
        auto &album = DataBase::albums[_album_uuid];
        auto &track = DataBase::tracks[uuid];
        if (album.tracks.contains(uuid)) {
            return false;
        }
        beginInsertRows(QModelIndex(), _tracks.size(), _tracks.size());
        album.tracks.append(uuid);
        track.albums.append(_album_uuid);
        _tracks.append(uuid);
        endInsertRows();
        return true;
    }
    return false;
}

bool TrackModel::moveTrackUp(const int row) {
    if (row < 1 || row >= _tracks.size())
        return false;
    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row - 1);
    std::swap(_tracks[row], _tracks[row - 1]);
    endMoveRows();
    return true;
}

bool TrackModel::moveTrackDown(const int row) {
    if (row < 0 || row >= _tracks.size() - 1)
        return false;
    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row + 2);
    std::swap(_tracks[row], _tracks[row + 1]);
    endMoveRows();
    return true;
}

/**
 *  根据选中单曲行号从专辑中删除单曲
 * @param row 选中单曲的行号
 * @return 删除成功/失败
 */
bool TrackModel::removeTrackFromAlbum(const int row) {
    if (row < 0 || row >= _tracks.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);

    // 从当前专辑中移除单曲
    const QString track_uuid = _tracks.at(row);
    _tracks.removeAt(row);
    auto &track = DataBase::tracks[track_uuid];
    track.removeFromAlbum(_album_uuid);
    endRemoveRows();

    return true;
}

/**
 * 根据行号获取选中单曲的 UUID
 * @param row 选中单曲的行号
 * @return 单曲的 UUID
 */
QString TrackModel::getTrackByRow(const int row) const {
    Q_ASSERT(row >= 0 && row < _tracks.size());
    return _tracks.at(row);
}

void TrackModel::clean() {
    beginResetModel();
    _album_uuid = {};
    _tracks = {};
    endResetModel();
}

bool TrackModel::isActive() const {
    return !_album_uuid.isEmpty();
}

void TrackModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置
    _tracks = DataBase::albums.keys().contains(_album_uuid) ? DataBase::albums[_album_uuid].tracks : QList<QString>{};
    endResetModel(); // 通知视图数据重置完成
}

QString TrackModel::getAlbum() {
    return _album_uuid;
}
