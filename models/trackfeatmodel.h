//
// Created by LEGION on 25-3-10.
//

#ifndef TRACKFEATMODEL_H
#define TRACKFEATMODEL_H

#include <QAbstractListModel>

class TrackFeatModel final : public QAbstractListModel {
Q_OBJECT
public:
    explicit TrackFeatModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    bool addData(const QString& uuid);

    bool removeData(int row);

    void setFamily(QList<QString> *feat_uuid_list, const QString& track_uuid);

    void clean();

    [[nodiscard]] bool isActive() const;

private:
    QList<QString> *feats{};

    QString track_uuid;

    QWidget *view{};
};



#endif //TRACKFEATMODEL_H
