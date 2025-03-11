//
// Created by LEGION on 25-3-10.
//

#ifndef TRACKFEATMODEL_H
#define TRACKFEATMODEL_H

#include <QAbstractListModel>

class TrackFeatModel : public QAbstractListModel {
Q_OBJECT
public:
    TrackFeatModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool addData(const QString& uuid);

    bool removeData(int row);

    void setFamily(QList<QString> *list, const QString& uuid);

    void clean();

    bool isActive();
private:
    QList<QString> *feats{};

    QString track_uuid;

    QWidget *view{};

    void refreshAll();
};



#endif //TRACKFEATMODEL_H
