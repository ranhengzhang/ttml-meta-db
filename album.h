//
// Created by LEGION on 25-3-5.
//

#ifndef ALBUM_H
#define ALBUM_H

#include "dataentity.h"

class Track;
class Album final :public DataEntity{
    friend Track;

public:
    Album();

    explicit Album(QJsonObject json);

    QList<QString> artists{};

    QList<QString> tracks{};

    void removeFromArtist(const QString& artist_uuid);

    [[nodiscard]] QJsonObject getSelf() override;

    bool isEmpty() override;

protected:
    [[nodiscard]]QMap<QString, QSet<QString>> getMetas() const override;
};



#endif //ALBUM_H
