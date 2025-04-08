//
// Created by LEGION on 25-4-8.
//

#include "artistmembermodel.h"

#include <QWidget>

#include "database.h"

ArtistMemberModel::ArtistMemberModel(QObject *parent) {
    view = dynamic_cast<QWidget *>(parent);
}

int ArtistMemberModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return members ? members->size() : 0;
}

QVariant ArtistMemberModel::data(const QModelIndex &index, int role) const {
    if (!members || !index.isValid() || index.row() >= members->size())
        return {};

    if (role == Qt::DisplayRole)
        return DataBase::artists[members->at(index.row())].getName();

    return {};
}

bool ArtistMemberModel::addData(QString artist_uuid) {
    if (DataBase::artists.contains(artist_uuid)) { // 检查是否存在歌手
        auto &artist = DataBase::artists[artist_uuid];
        if (artist.members.contains(artist_uuid)) { // 检查是否已经添加过
            return false;
        }

        beginInsertRows(QModelIndex(), members->size(), members->size());
        artist.albums.append(artist_uuid);
        members->append(artist_uuid);
        endInsertRows();
        return true;
    }
    return false;
}

bool ArtistMemberModel::removeData(const int row) {
    if (row < 0 || row >= members->size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);
    members->removeAt(row);
    endRemoveRows();
    return true;
}

void ArtistMemberModel::setFamily(QList<QString> *artist_member_list, const QString &uuid) {
    beginResetModel();
    members = artist_member_list;
    artist_uuid = uuid;
    endResetModel();
}

void ArtistMemberModel::clean() {
    beginResetModel();
    members = nullptr;
    artist_uuid = {};
    endResetModel();
}

bool ArtistMemberModel::isActive() const {
    return members != nullptr;
}
