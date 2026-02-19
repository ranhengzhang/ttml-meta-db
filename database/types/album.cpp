//
// Created by LEGION on 25-3-5.
//

#include "album.h"
#include "artist.h"
#include "../database.h"

#include <QJsonArray>

Album::Album() {
    self["artists"] = QJsonArray();
    self["tracks"] = QJsonArray();
}

Album::Album(QJsonObject json) : DataEntity(json) {
    // 添加专辑对应的歌手
    if (json.contains("artists") && json["artists"].isArray()) {
        auto artist_list = json["artists"].toArray();
        for (auto artist: artist_list) {
            artists.append(artist.toString());
        }
    } else {
        // 没有歌手
        self["artists"] = QJsonArray();
    }
    // 添加专辑包含的单曲
    if (json.contains("tracks") && json["tracks"].isArray()) {
        auto track_list = json["tracks"].toArray();
        for (auto track: track_list) {
            tracks.append(track.toString());
        }
    } else {
        // 没有单曲
        self["tracks"] = QJsonArray();
    }
}

/**
 * 以 key-valueSet 的形式返回专辑名以及所有歌手的歌手名
 * \code{.json}
 * {
 *   "album": set<str>["专辑名1","专辑名2"],
 *   "artists": set<str>["歌手名1","歌手名2"],
 * }
 * \endcode
 * @return 元数据 map
 */
QMap<QString, QList<QString>> Album::getMetas() const {
    QMap<QString, QList<QString> > metadata{};

    // 添加专辑名
    for (const auto &meta: metas) {
        if (!metadata["album"].contains(meta))
            metadata["album"].push_back(meta);
    }

    // 合并歌手信息
    for (const auto &artist: artists) {
        for (const auto& artist_name: DataBase::artists[artist].getMetas()["artists"])
            if (!metadata["artists"].contains(artist_name))
                metadata["artists"].push_back(artist_name);
    }

    return metadata;
}

/**
 * 删除专辑中的对应歌手
 * 如果专辑悬空则销毁
 * @param artist_uuid 歌手 UUID
 */
void Album::removeFromArtist(const QString &artist_uuid) {
    if (artists.contains(artist_uuid)) {
        artists.removeAll(artist_uuid);
        auto &artist_object = DataBase::artists[artist_uuid];
        artist_object.albums.removeAll(UUID);
        destroyIfOrphan();
    }
}

QJsonObject Album::getSelf() {
    setSelf();
    self["artists"] = QJsonArray::fromStringList(artists);
    self["tracks"] = QJsonArray::fromStringList(tracks);
    return self;
}

bool Album::isEmpty() {
    return DataEntity::isEmpty() && this->artists.isEmpty() && this->tracks.isEmpty();
}

void Album::destroyIfOrphan() {
    if (artists.isEmpty() && DataBase::albums.keys().contains(UUID)) {
        for (auto &track: tracks) {
            if (DataBase::tracks.keys().contains(track)) {
                auto &track_object = DataBase::tracks[track];
                track_object.removeFromAlbum(UUID);
            }
        }
        DataBase::albums.remove(UUID);
    }
}
