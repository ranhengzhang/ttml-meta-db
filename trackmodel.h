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

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    void setAlbum(const QString &uuid);

    bool addNewData(const QString& name);

    bool addOldData(const QString& uuid);

    bool moveTrackUp(int row);

    bool moveTrackDown(int row);

    bool removeTrackFromAlbum(int row);

    [[nodiscard]] QString getTrackByRow(int row) const;

    void clean();

    [[nodiscard]] bool isActive() const;

    void refreshAll();

    [[nodiscard]]QString getAlbum();

private:
    QString album_uuid;

    QList<QString> tracks{};

    QWidget *window;
};



#endif //TRACKMODEL_H
