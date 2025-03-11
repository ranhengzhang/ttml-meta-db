//
// Created by LEGION on 25-3-10.
//

#ifndef TRACKALBUMMODEL_H
#define TRACKALBUMMODEL_H

#include <QAbstractListModel>

class TrackAlbumModel : public QAbstractListModel {
Q_OBJECT
public:
    TrackAlbumModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool addData(const QString& uuid);

    bool removeData(int row);

    void setFamily(QList<QString> *list, const QString& uuid);

    void clean();

    bool isActive();
private:
    QList<QString> *albums{};

    QString track_uuid;

    QWidget *view{};

    void refreshAll();
};



#endif //TRACKALBUMMODEL_H
