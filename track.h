//
// Created by LEGION on 25-3-5.
//

#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <QMap>

#include "dataentity.h"

class Track final :public DataEntity{
public:
    Track();
    explicit Track(QJsonObject json);

    QList<QString> albums; // 专辑列表

    QList<QString> feats; // 合作艺人列表

    QList<QList<QString>> ids; // 歌曲ID列表

    void removeFromAlbum(const QString& album_uuid);

    [[nodiscard]] QJsonObject getSelf() override;

    [[nodiscard]] QStringList printMeta() const;

protected:
    [[nodiscard]] QMap<QString, QSet<QString>> getMetas() const override; // 导出xml
};



#endif //TRACK_H
