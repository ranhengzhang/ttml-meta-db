//
// Created by LEGION on 25-3-10.
//

#ifndef ALBUMARTISTMODEL_H
#define ALBUMARTISTMODEL_H

#include <QAbstractListModel>

#include "dataentity.h"

class AlbumArtistModel final : public QAbstractListModel {
Q_OBJECT
public:
    explicit AlbumArtistModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    bool addData(const QString& artist_uuid);

    bool removeData(int row);

    void setFamily(QList<QString> *album_artist_list, const QString& uuid);

    void clean();

    [[nodiscard]] bool isActive() const;

private:
    QList<QString> *artists{};

    QString album_uuid;

    QWidget *view{};
};



#endif //ALBUMARTISTMODEL_H
