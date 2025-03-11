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
    if (json.contains("artists") && json["artists"].isArray()) {
        auto artistsList = json["artists"].toArray();
        for (auto artist:artistsList) {
            artists.append(artist.toString());
        }
    } else {
        self["artists"] = QJsonArray();
    }
    if (json.contains("tracks") && json["tracks"].isArray()) {
        auto trackList = json["tracks"].toArray();
        for (auto track:trackList) {
            tracks.append(track.toString());
        }
    } else {
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
 * 从歌手中删除专辑
 * 如果专辑悬空则销毁
 * @param uuid 歌手 UUID
 */
void Album::removeFromArtist(const QString& uuid) {
    artists.removeAll(uuid);

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
