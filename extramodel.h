//
// Created by LEGION on 25-8-1.
//

#ifndef EXTRAMODEL_H
#define EXTRAMODEL_H

#include <QTableView>

class ExtraModel : public QAbstractTableModel {
Q_OBJECT
public:
    explicit ExtraModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool addData(const QString& key, const QString& value);

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent) override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    [[nodiscard]] bool isActive() const;

    void setFamily(QList<QList<QString>> *meta_list, const QString& track_uuid);

    void clean();

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QList<QList<QString>> *extras{};

    QString track_uuid{};

    QTableView *view{};
};



#endif //EXTRAMODEL_H
