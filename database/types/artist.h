//
// Created by LEGION on 25-3-5.
//

#ifndef ARTIST_H
#define ARTIST_H

#include "dataentity.h"

class Album;
class Track;
class Artist final :public DataEntity{
    friend Album;
    friend Track;

public:
    Artist();

    explicit Artist(QJsonObject json);

    QList<QString> albums{};

    QList<QString> members{};

    void remove();

    [[nodiscard]] QJsonObject getSelf() override;

    bool isEmpty() override;

    void destroyIfOrphan() override;

protected:
    [[nodiscard]] QMap<QString, QList<QString>> getMetas() const override;
};

#endif //ARTIST_H
