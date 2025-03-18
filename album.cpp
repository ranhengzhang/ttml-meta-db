//
// Created by LEGION on 25-3-5.
//

#include "album.h"
#include "artist.h"
#include "database.h"

#include <QJsonArray>

Album::Album() {
    self["artists"] = QJsonArray();
    self["tracks"] = QJsonArray();
}

Album::Album(QJsonObject json): DataEntity(json) {
    // 添加专辑对应的歌手
    if (json.contains("artists") && json["artists"].isArray()) {
        auto artist_list = json["artists"].toArray();
        for (auto artist:artist_list) {
            artists.append(artist.toString());
        }
    } else {
        // 没有歌手
        self["artists"] = QJsonArray();
    }
    // 添加专辑包含的单曲
    if (json.contains("tracks") && json["tracks"].isArray()) {
        auto track_list = json["tracks"].toArray();
        for (auto track:track_list) {
            tracks.append(track.toString());
        }
    } else {
        // 没有单曲
        self["tracks"] = QJsonArray();
    }
}

QList<QString> Album::toXML() const {
    QList<QString> xml{};

    // 添加专辑名
    for (auto &meta:metas) {
        xml.append(QString(R"(<amll:meta key="album" value="%1" />)").arg(meta));
    }

    // 合并歌手信息
    for (const auto& artist : artists) {
        xml.append(DataBase::artists[artist].toXML());
    }

    return xml;
}

/**
 * 删除专辑中的对应歌手
 * 如果专辑悬空则销毁
 * @param artist_uuid 歌手 UUID
 */
void Album::removeFromArtist(const QString& artist_uuid) {
    artists.removeAll(artist_uuid);

    // 销毁没有歌手的专辑
    if (artists.isEmpty()) {
        // 从歌曲中清除专辑
        for( auto &track_uuid: tracks) {
            auto &track = DataBase::tracks[track_uuid];
            track.removeFromAlbum(UUID);
        }

        // 销毁
        DataBase::albums.remove(UUID);
    }
}

QJsonObject Album::getSelf() {
    setSelf();
    self["artists"] = QJsonArray::fromStringList(artists);
    self["tracks"] = QJsonArray::fromStringList(tracks);
    return self;
}
