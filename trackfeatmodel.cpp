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

void TrackFeatModel::setFamily(QList<QString> *list, const QString &uuid) {
    feats = list;
    track_uuid = uuid;

    refreshAll();
}

void TrackFeatModel::clean() {
    feats = nullptr;
    track_uuid = {};
    refreshAll();
}

bool TrackFeatModel::isActive() {
    return feats != nullptr;
}

void TrackFeatModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置
    endResetModel();   // 通知视图数据重置完成
}
