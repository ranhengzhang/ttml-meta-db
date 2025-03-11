//
// Created by LEGION on 25-3-8.
//

#ifndef ARTISTMODEL_H
#define ARTISTMODEL_H

#include <QAbstractListModel>

class Artist;
class ArtistModel final : public QAbstractListModel{
Q_OBJECT
public:
    explicit ArtistModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool addData(const QString& name);

    bool removeArtist(int row);

    QString getArtistByRow(int row) const;

    void refreshAll();
private:
    QList<QString> artists;

    QWidget *view;
};



#endif //ARTISTMODEL_H
