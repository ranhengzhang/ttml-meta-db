//
// Created by LEGION on 25-3-10.
//

#ifndef TRACKALBUMMODEL_H
#define TRACKALBUMMODEL_H

#include <QAbstractListModel>

class TrackAlbumModel final : public QAbstractListModel {
Q_OBJECT
public:
    explicit TrackAlbumModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    bool addData(const QString& uuid);

    bool removeData(int row);

    void setFamily(QList<QString> *album_list, const QString& track_uuid);

    void clean();

    [[nodiscard]] bool isActive() const;

private:
    QList<QString> *albums{};

    QString track_uuid;

    QWidget *view{};
};



#endif //TRACKALBUMMODEL_H
