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
}

QMap<QString, QSet<QString>> Artist::getMetas() const {
    QMap<QString, QSet<QString>> metadata{};

    // 添加歌手名
    for (auto &meta:metas) {
        metadata["artists"].insert(meta);
    }

    return metadata;
}

void Artist::remove() {
    // 从所有专辑中删除歌手
    for (auto &album_uuid : albums) {
        auto &album = DataBase::albums[album_uuid];
        album.removeFromArtist(UUID);
    }

    // 从库中删除
    DataBase::artists.remove(UUID);
}

QJsonObject Artist::getSelf() {
    setSelf();
    self["albums"] = QJsonArray::fromStringList(albums);
    return self;
}
