//
// Created by LEGION on 25-3-5.
//

#ifndef ALBUM_H
#define ALBUM_H

#include "dataentity.h"

class Album final :public DataEntity{
public:
    Album();
    explicit Album(QJsonObject json);

    QList<QString> artists{};
    QList<QString> tracks{};

    [[nodiscard]]QList<QString> toXML() const override;
    void removeFromArtist(const QString& uuid);

    [[nodiscard]] QJsonObject getSelf() override;
};



#endif //ALBUM_H
