//
// Created by LEGION on 25-4-8.
//

#ifndef ARTISTMEMBERMODEL_H
#define ARTISTMEMBERMODEL_H

#include <QAbstractListModel>

class ArtistMemberModel final : public QAbstractListModel {
Q_OBJECT
public:
    explicit ArtistMemberModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    bool addData(QString artist_uuid);

    bool removeData(int row);

    void setFamily(QList<QString> *artist_member_list, const QString& uuid);

    void clean();

    [[nodiscard]] bool isActive() const;

private:
    QList<QString> *members{};

    QString artist_uuid;

    QWidget *view{};
};



#endif //ARTISTMEMBERMODEL_H
