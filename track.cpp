//
// Created by LEGION on 25-3-5.
//

#include "album.h"
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

QList<QString> Track::toXML() const {
    QSet<QString> xml{};

    // 添加歌曲名
    for (auto &meta:metas) {
        xml.insert(QString(R"(<amll:meta key="musicName" value="%1" />)").arg(meta));
    }

    // 添加ID
    for (const auto&id: ids) {
        xml.insert(QString(R"(<amll:meta key="%1" value="%2" />)").arg(id[0]).arg(id[1]));
    }

    // 添加合作歌手
    for (const auto& uuid: feats) {
        auto feat = DataBase::artists[uuid];
        for (const auto& info: feat.toXML()) {
            xml.insert(info);
        }
    }

    // 合并专辑信息
    for (const auto& uuid : albums) {
        auto album = DataBase::albums[uuid];
        for (const auto& info : album.toXML()) {
            xml.insert(info);
        }
    }

    QStringList metas = {xml.begin(), xml.end()};
    metas.sort();
    // 返回
    return metas;
}

void Track::removeFromAlbum(const QString& uuid) {
    albums.removeAll(uuid);
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
