//
// Created by LEGION on 25-3-5.
//

#ifndef ARTIST_H
#define ARTIST_H

#include "dataentity.h"

class Artist final :public DataEntity{
public:
    Artist();

    explicit Artist(QJsonObject json);

    QList<QString> albums{};

    [[nodiscard]]QList<QString> toXML() const override;

    void remove();

    [[nodiscard]] QJsonObject getSelf() override;
};

#endif //ARTIST_H
