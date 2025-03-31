//
// Created by LEGION on 25-3-7.
//

#ifndef DATAENTITY_H
#define DATAENTITY_H

#include <QJsonObject>
#include <QString>
#include <QSet>

class DataEntity {
public:
    QList<QString> metas; // 名称列表

    explicit DataEntity(QJsonObject &json);

    virtual ~DataEntity() = default;

    [[nodiscard]] QString getUUID() const;

    [[nodiscard]] virtual QJsonObject getSelf() = 0;

    [[nodiscard]] QString getName() const;

    [[nodiscard]] virtual QMap<QString, QSet<QString>> toXML() const = 0;

    void setSelf();

protected:
    QString UUID;

    QJsonObject self; // json对象

    DataEntity();
};



#endif //DATAENTITY_H
