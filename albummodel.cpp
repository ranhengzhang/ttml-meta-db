//
// Created by LEGION on 25-3-9.
//

#include "albummodel.h"

#include <QWidget>

#include "database.h"

#include "artist.h"

AlbumModel::AlbumModel(QObject *parent) : QAbstractListModel(parent) {
    _view = dynamic_cast<QWidget *>(parent);
}

int AlbumModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return _albums.size();
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= _albums.size())
        return {};

    const QString uuid = _albums.at(index.row());
    if (!DataBase::albums.contains(uuid)) {
        return {};
    }
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
    auto &artist = DataBase::artists[_artist_uuid];

    // 检查歌手下同名专辑
    bool exist = false;
    for (auto &album_uuid: artist.albums) {
        exist |= DataBase::albums[album_uuid].metas.contains(name);
    }
    if (exist) {
        return false; // 名称已存在，插入失败
    }

    int index = 0;
    for (index = 0; index < _albums.size(); ++index) {
        if (DataBase::albums[_albums[index]].getName() > name)
            break;
    }

    beginInsertRows(QModelIndex(), index, index);

    Album val;
    val.metas.append(name);
    val.artists.append(_artist_uuid);

    DataBase::albums.insert(val.getUUID(), val);
    artist.albums.append(val.getUUID());
    _albums.append(val.getUUID());

    endInsertRows();
    return true; // 插入成功
}

/**
 * 将当前歌手添加到一张已有专辑中
 * @param album_uuid 已有专辑 UUID
 * @return 添加成功/失败
 */
bool AlbumModel::addOldData(const QString &album_uuid) {
    if (DataBase::albums.contains(album_uuid)) {
        auto &artist = DataBase::artists[_artist_uuid];
        if (artist.albums.contains(album_uuid)) {
            return false;
        }

        beginInsertRows(QModelIndex(), _albums.size(), _albums.size());
        auto &album = DataBase::albums[album_uuid];
        artist.albums.append(album_uuid);
        album.artists.append(_artist_uuid);
        _albums.append(album_uuid);
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
    if (row < 0 || row >= _albums.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);
    // 从当前歌手中移除专辑
    auto &artist = DataBase::artists[_artist_uuid];
    const QString album_uuid = _albums.at(row);
    auto &album = DataBase::albums[album_uuid];
    artist.albums.removeAll(album_uuid);
    album.removeFromArtist(_artist_uuid);
    endRemoveRows();

    return true;
}

/**
 * 设置当前的歌手并列出所有专辑
 * @param artist_uuid 歌手 UUID
 */
void AlbumModel::setArtist(const QString &artist_uuid) {
    beginResetModel();
    this->_artist_uuid = artist_uuid;

    // 将歌手的专辑添加到显示中
    auto &artist = DataBase::artists[artist_uuid];
    for (auto &album_uuid: artist.albums) {
        // 去除不存在的专辑
        if (!DataBase::albums.contains(album_uuid)) {
            artist.albums.removeAll(album_uuid);
        }
    }
    _albums = artist.albums;
    std::sort(_albums.begin(), _albums.end(), [](const QString &keyA, const QString &keyB) {
        return DataBase::albums[keyA].getName() < DataBase::albums[keyB].getName();
    });
    endResetModel();
}

void AlbumModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置

    _albums = DataBase::artists.contains(_artist_uuid) ? DataBase::artists[_artist_uuid].albums : QList<QString>{};

    std::sort(_albums.begin(), _albums.end(), [](const QString &keyA, const QString &keyB) {
        return DataBase::albums[keyA].getName() < DataBase::albums[keyB].getName();
    });

    endResetModel(); // 通知视图数据重置完成
}

/**
 * 根据行号获取专辑 UUID
 * @param row 选中专辑的行号
 * @return 专辑的 UUID
 */
QString AlbumModel::getAlbumByRow(const int row) const {
    Q_ASSERT(row >= 0 && row < _albums.size());
    return _albums.at(row);
}

void AlbumModel::clean() {
    beginResetModel();
    _artist_uuid = {};
    _albums = {};
    endResetModel();
}

QString AlbumModel::getArtist() const {
    return _artist_uuid;
}
