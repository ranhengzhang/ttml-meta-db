//
// Created by LEGION on 25-3-9.
//

#include "albummodel.h"

#include <QWidget>

#include "database.h"

#include "artist.h"

AlbumModel::AlbumModel(QObject *parent): QAbstractListModel(parent) {
    window = dynamic_cast<QWidget *>(parent);
}

int AlbumModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return albums.size();
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= albums.size())
        return {};

    const QString uuid = albums.at(index.row());
    const Album &album = DataBase::albums[uuid];

    if (role == Qt::DisplayRole)
        return album.getName();

    return {};
}

Qt::ItemFlags AlbumModel::flags(const QModelIndex &index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

/**
 * 为当前歌手添加一张新专辑
 * @param name 专辑名称
 * @return 添加成功/失败
 */
bool AlbumModel::addNewData(const QString &name) {
    // 获取当前歌手
    auto &artist = DataBase::artists[artist_uuid];

    // 检查歌手下同名专辑
    bool exist = false;
    for (auto &album_uuid: artist.albums) {
        exist |= DataBase::albums[album_uuid].metas.contains(name);
    }
    if (exist) {
        return false; // 名称已存在，插入失败
    }

    beginInsertRows(QModelIndex(), albums.size(), albums.size());
    Album val;

    val.metas.append(name);
    val.artists.append(artist_uuid);
    DataBase::albums.insert(val.getUUID(), val);
    artist.albums.append(val.getUUID());
    albums.append(val.getUUID());
    endInsertRows();
    return true; // 插入成功
}

/**
 * 将当前歌手添加到一张已有专辑中
 * @param uuid 已有专辑 UUID
 * @return 添加成功/失败
 */
bool AlbumModel::addOldData(const QString& uuid) {
    if (DataBase::albums.contains(uuid)) {
        auto &artist = DataBase::artists[artist_uuid];
        auto &album = DataBase::albums[uuid];
        if (artist.albums.contains(uuid)) {
            return false;
        }
        beginInsertRows(QModelIndex(), albums.size(), albums.size());
        artist.albums.append(uuid);
        album.artists.append(artist_uuid);
        albums.append(uuid);
        endInsertRows();
        return true;
    }
    return false;
}

/**
 * 从歌手中删除选中专辑
 * @param row 选中专辑在 model 中的行号
 * @return 删除成功/失败
 */
bool AlbumModel::removeAlbumFromArtist(const int row) {
    if (row < 0 || row >= albums.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);

    // 从当前歌手中移除专辑
    const QString album_uuid = albums.at(row);
    auto &album = DataBase::albums[album_uuid];
    album.removeFromArtist(artist_uuid);
    endRemoveRows();

    return true;
}

/**
 * 设置当前的歌手并列出所有专辑
 * @param uuid 歌手 UUID
 */
void AlbumModel::setArtist(const QString& uuid) {
    artist_uuid = uuid;
    const auto &artist = DataBase::artists[uuid];

    // 将歌手的专辑添加到显示中
    albums = artist.albums;

    refreshAll();
}

void AlbumModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置
    albums = DataBase::artists.contains(artist_uuid) ? DataBase::artists[artist_uuid].albums : QList<QString>{};
    endResetModel();   // 通知视图数据重置完成
}

QString AlbumModel::getAlbumByRow(const int row) const {
    Q_ASSERT(row >= 0 && row < albums.size());
    return albums.at(row);
}

void AlbumModel::clean() {
    artist_uuid = {};
    albums = {};
    refreshAll();
}
