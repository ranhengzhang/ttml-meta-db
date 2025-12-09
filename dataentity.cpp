//
// Created by LEGION on 25-3-7.
//

#include "dataentity.h"

#include <QJsonArray>

DataEntity::DataEntity() : UUID(QUuid::createUuid().toString(QUuid::WithoutBraces)), self(QJsonObject()) {
    self["metas"] = QJsonArray();
}

bool DataEntity::isEmpty() {
    return this->metas.isEmpty();
}

DataEntity::DataEntity(QJsonObject &json) {
    self = json;
    UUID = json["uuid"].toString();
    auto metaList = json["metas"].toArray();
    for (auto meta: metaList) {
        metas.append(meta.toString());
    }
}

QString DataEntity::getUUID() const { return UUID; }

void DataEntity::setSelf() {
    self["uuid"] = QJsonValue(UUID);
    self["metas"] = QJsonArray::fromStringList(metas);
}

QString DataEntity::getName() const { return metas.empty() ? UUID : *metas.begin(); }
