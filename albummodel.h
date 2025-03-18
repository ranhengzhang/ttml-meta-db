//
// Created by LEGION on 25-3-9.
//

#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H

#include <QAbstractListModel>

class AlbumModel final : public QAbstractListModel {
Q_OBJECT
public:
    explicit AlbumModel(QObject *parent = nullptr);

public:
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool addNewData(const QString& name);

    bool addOldData(const QString &uuid);

    bool removeAlbumFromArtist(int row);

    void setArtist(const QString& uuid);

    void refreshAll();

    QString getAlbumByRow(int row) const;

    void clean();

    [[nodiscard]]QString getArtist() const;

private:
    QString artist_uuid;

    QList<QString> albums{};

    QWidget *window;
};



#endif //ALBUMMODEL_H
