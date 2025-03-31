//
// Created by LEGION on 25-3-5.
//

#include "track.h"
#include "database.h"

#include <QJsonArray>

Track::Track() {
    self["albums"] = QJsonArray();
}

Track::Track(QJsonObject json):DataEntity(json) {
    if (json.contains("albums") && json["albums"].isArray()) {
        auto albumsList = json["albums"].toArray();
        for (auto album:albumsList) {
            albums.append(album.toString());
        }
    } else {
        self["albums"] = QJsonArray();
    }
    if (json.contains("feats") && json["feats"].isArray()) {
        auto featList = json["feats"].toArray();
        for (auto feat:featList) {
            feats.append(feat.toString());
        }
    } else {
        self["feats"] = QJsonArray();
    }
    if (json.contains("ids") && json["ids"].isArray()) {
        auto idList = json["ids"].toArray();
        for (auto pair:idList) {
            auto id = pair.toObject();
            ids.append({id["key"].toString(), id["value"].toString()});
        }
    } else {
        self["ids"] = QJsonArray();
    }
}

QMap<QString, QSet<QString>> Track::toXML() const {
    QMap<QString, QSet<QString>> xml{};

    // 添加歌曲名
    for (auto &meta:metas) {
        xml["musicName"].insert(meta);
    }

    // 添加ID
    for (const auto&id: ids) {
        xml[id[0]].insert(id[1]);
    }

    // 添加合作歌手
    for (const auto& uuid: feats) {
        auto feat = DataBase::artists[uuid];
        xml["artists"].unite(feat.toXML()["artists"]);
    }

    // 合并专辑信息
    for (const auto& uuid : albums) {
        auto album = DataBase::albums[uuid];
        auto info = album.toXML();

        xml["artists"].unite(info["artists"]);
        xml["album"].unite(info["album"]);
    }
    // 返回
    return xml;
}

/**
 * 从当前专辑删除单曲
 * @param album_uuid 专辑 UUID
 */
void Track::removeFromAlbum(const QString& album_uuid) {
    albums.removeAll(album_uuid);
    if (albums.isEmpty())
        DataBase::tracks.remove(UUID);
}

QJsonObject Track::getSelf() {
    setSelf();
    self["albums"] = QJsonArray::fromStringList(albums);
    self["feats"] = QJsonArray::fromStringList(feats);

    QJsonArray idsArray;
    for (auto &pair: ids) {
        QJsonObject obj;
        obj["key"] = pair[0];
        obj["value"] = pair[1];
        idsArray.append(obj);
    }
    self["ids"] = idsArray;
    return self;
}

QStringList Track::getMetas() {
    auto xml = this->toXML();
    QStringList metas{};
    const QStringList keys = {
        "musicName",
        "artists",
        "album",
        "ncmMusicId",
        "qqMusicId",
        "spotifyId",
        "appleMusicId",
        "isrc"
    };

    for (auto &key : keys) {
        for (auto &value: xml[key]) {
            metas.append(QString(R"(<amll:meta key="%1" value="%2" />)").arg(key).arg(value));
        }
    }

    return metas;
}
