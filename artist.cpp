//
// Created by LEGION on 25-3-5.
//

#include "artist.h"

#include <QJsonArray>

#include "album.h"
#include "database.h"

Artist::Artist() {
    self["albums"] = QJsonArray();
}

Artist::Artist(QJsonObject json): DataEntity(json) {
    if (json.contains("albums") && json["albums"].isArray()) {
        auto albumList = json["albums"].toArray();
        for (auto album:albumList) {
            albums.append(album.toString());
        }
    } else {
       self["albums"] = QJsonArray();
    }

    if (json.contains("members") && json["members"].isArray()) {
        auto artistList = json["members"].toArray();
        for (auto artist :artistList) {
            members.append(artist.toString());
        }
    } else {
        self["members"] = QJsonArray();
    }
}

/**
 * 以 key-valueSet 的形式返回歌手名
 * \code{.json}
 * {
 *     "artists": set<str>["歌手名1","歌手名2"],
 * }
 * \endcode
 * @return 元数据 map
 */
QMap<QString, QSet<QString>> Artist::getMetas() const {
    QMap<QString, QSet<QString>> metadata{};

    // 添加歌手名
    for (auto &meta:metas) {
        metadata["artists"].insert(meta);
    }

    return metadata;
}

bool Artist::isEmpty() {
    return DataEntity::isEmpty() && this->albums.isEmpty() && this->members.isEmpty();
}

void Artist::remove() {
    // 从所有专辑中删除歌手
    for (auto &album_uuid : albums) {
        auto &album = DataBase::albums[album_uuid];
        album.removeFromArtist(UUID);
    }

    // 从所有单曲的 feat 中删除歌手
    const auto trackList = DataBase::tracks.keys();
    for (auto &track_uuid : trackList) {
        auto &track = DataBase::tracks[track_uuid];
        if (track.feats.contains(UUID)) {
            track.feats.removeAll(this->UUID);
        }
    }

    // 从所有组合的 member 中删除歌手
    const auto artistList = DataBase::artists.keys();
    for (auto &artist_uuid : artistList) {
        auto &artist = DataBase::artists[artist_uuid];
        if (artist.members.contains(UUID)) {
            artist.members.removeAll(this->UUID);
        }
    }

    // 从库中删除
    DataBase::artists.remove(UUID);
}

QJsonObject Artist::getSelf() {
    setSelf();
    self["albums"] = QJsonArray::fromStringList(albums);
    return self;
}
