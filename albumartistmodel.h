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

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool addData(const QString& uuid);

    bool removeData(int row);

    void setFamily(QList<QString> *list, const QString& uuid);

    void clean();

    bool isActive() const;
private:
    QList<QString> *artists{};

    QString album_uuid;

    QWidget *view{};

    void refreshAll();
};



#endif //ALBUMARTISTMODEL_H
