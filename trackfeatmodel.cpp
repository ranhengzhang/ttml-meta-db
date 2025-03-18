//
// Created by LEGION on 25-3-10.
//

#include "trackfeatmodel.h"

#include <QWidget>

#include "artist.h"
#include "database.h"

TrackFeatModel::TrackFeatModel(QObject *parent) {
    view = dynamic_cast<QWidget *>(parent);
}

int TrackFeatModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return feats ? feats->size() : 0;
}

QVariant TrackFeatModel::data(const QModelIndex &index, int role) const {
    if (!feats || !index.isValid() || index.row() >= feats->size())
        return {};

    const QString uuid = feats->at(index.row());
    const Artist &artist = DataBase::artists[uuid];

    if (role == Qt::DisplayRole)
        return artist.getName();

    return {};
}

bool TrackFeatModel::addData(const QString &uuid) {
    if (DataBase::artists.contains(uuid)) {
        if (DataBase::tracks[track_uuid].feats.contains(uuid))
            return false;
        auto &artist = DataBase::artists[uuid];
        beginInsertRows(QModelIndex(), feats->size(), feats->size());
        feats->append(uuid);
        endInsertRows();
        return true;
    }
    return false;
}

bool TrackFeatModel::removeData(int row) {
    if (row < 0 || row >= feats->size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);

    // 从选中歌手中移除专辑
    const QString artist_uuid = feats->at(row);
    auto &track = DataBase::tracks[track_uuid];
    track.feats.removeAll(artist_uuid);
    endRemoveRows();

    return true;
}

/**
 * 设置当前单曲的 FEAT 和 UUID
 * @param feat_list FEAT UUID 列表
 * @param track_uuid 单曲的 UUID
 */
void TrackFeatModel::setFamily(QList<QString> *feat_list, const QString &track_uuid) {
    beginResetModel();
    feats = feat_list;
    this->track_uuid = track_uuid;
    endResetModel();
}

void TrackFeatModel::clean() {
    beginResetModel();
    feats = nullptr;
    track_uuid = {};
    endResetModel();
}

bool TrackFeatModel::isActive() const {
    return feats != nullptr;
}
