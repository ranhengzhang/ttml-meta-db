//
// Created by LEGION on 25-3-5.
//

#include "track.h"
#include "database.h"

#include <QJsonArray>

#include "utils.h"

Track::Track() {
    self["albums"] = QJsonArray();
}

Track::Track(QJsonObject json) : DataEntity(json) {
    if (json.contains("albums") && json["albums"].isArray()) {
        auto albumsList = json["albums"].toArray();
        for (auto album: albumsList) {
            albums.append(album.toString());
        }
    } else {
        self["albums"] = QJsonArray();
    }
    if (json.contains("feats") && json["feats"].isArray()) {
        auto featList = json["feats"].toArray();
        for (auto feat: featList) {
            feats.append(feat.toString());
        }
    } else {
        self["feats"] = QJsonArray();
    }
    if (json.contains("ids") && json["ids"].isArray()) {
        auto idList = json["ids"].toArray();
        for (auto pair: idList) {
            auto id = pair.toObject();
            ids.append({id["key"].toString(), id["value"].toString()});
        }
    } else {
        self["ids"] = QJsonArray();
    }
    if (json.contains("extras") && json["extras"].isArray()) {
        auto extList = json["extras"].toArray();
        for (auto pair: extList) {
            auto ext = pair.toObject();
            extras.append({ext["key"].toString(), ext["value"].toString()});
        }
    } else {
        self["extras"] = QJsonArray();
    }
}

QMap<QString, QList<QString>> Track::getMetas() const {
    QMap<QString, QList<QString> > metadata{};

    // 添加歌曲名
    for (const auto &meta: metas) {
        if (!metadata["musicName"].contains(meta))
            metadata["musicName"].push_back(meta);
    }

    // 添加ID
    for (const auto &id: ids) {
        if (!metadata[id[0]].contains(id[1]))
            metadata[id[0]].push_back(id[1]);
    }

    // 添加额外信息
    for (const auto &ext: extras) {
        if (!metadata[ext[0]].contains(ext[1]))
            metadata[ext[0]].push_back(ext[1]);
    }

    // 添加合作歌手
    for (const auto &uuid: feats) {
        auto feat = DataBase::artists[uuid];

        for (const auto &artist: feat.getMetas()["artists"])
            if (!metadata["artists"].contains(artist))
                metadata["artists"].push_back(artist);
    }

    // 合并专辑信息
    for (const auto &uuid: albums) {
        auto album = DataBase::albums[uuid];
        auto info = album.getMetas();

        for (const auto &artist: info["artists"])
            if (!metadata["artists"].contains(artist))
                metadata["artists"].push_back(artist);

        for (const auto &album_title: info["album"])
            if (!metadata["album"].contains(album_title))
                metadata["album"].push_back(album_title);
    }
    // 返回
    return metadata;
}

/**
 * 从当前专辑删除单曲
 * @param album_uuid 专辑 UUID
 */
void Track::removeFromAlbum(const QString &album_uuid) {
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

    QJsonArray extArray;
    for (auto &ext: extras) {
        QJsonObject obj;
        obj["key"] = ext[0];
        obj["value"] = ext[1];
        extArray.append(obj);
    }
    self["extras"] = extArray;

    return self;
}


/**
 * 获取当前歌曲包含的元数据，排序后返回
 * \code{.xml}
 * <amll:meta key="%1" value="%2" />
 * \endcode
 * @return xml 格式并且排序好的元数据
 */
QStringList Track::printMeta() const {
    const auto xml = this->getMetas();
    QStringList preMeta{};
    QStringList extMeta{};
    const QStringList keys = {
        // ID 顺序
        "musicName",
        "artists",
        "album",
        "ncmMusicId",
        "qqMusicId",
        "spotifyId",
        "appleMusicId",
        "isrc"
    };

    for (auto &key: keys)
        for (auto &value: xml[key])
            preMeta.append(
                QString(R"(<amll:meta key="%1" value="%2"/>)").arg(utils::toHtmlEscaped(key)).arg(utils::toHtmlEscaped(value)));

    for (auto &[key, values]: xml.toStdMap())
        if (!keys.contains(key))
            for (auto &value: values)
                extMeta.append(
                    QString(R"(<amll:meta key="%1" value="%2"/>)").arg(utils::toHtmlEscaped(key)).arg(
                        utils::toHtmlEscaped(value)));

    return preMeta + extMeta;
}

bool Track::isEmpty() {
    return DataEntity::isEmpty() && this->albums.isEmpty() && this->feats.isEmpty() && this->ids.isEmpty();
}
