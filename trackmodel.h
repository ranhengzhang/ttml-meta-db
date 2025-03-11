//
// Created by LEGION on 25-3-10.
//

#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QAbstractListModel>

class TrackModel final : public QAbstractListModel {
Q_OBJECT
public:
    explicit TrackModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void setAlbum(const QString &uuid);

    bool addNewData(QString name);

    bool addOldData(const QString& uuid);

    bool removeTrackFromAlbum(int row);

    QString getTrackByRow(int row) const;

    void clean();

    bool isActive() const;

    void refreshAll();

private:
    QString album_uuid;

    QList<QString> tracks{};

    QWidget *window;
};



#endif //TRACKMODEL_H
