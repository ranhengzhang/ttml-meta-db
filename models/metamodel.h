//
// Created by LEGION on 25-3-8.
//

#ifndef METAMODEL_H
#define METAMODEL_H

#include <QAbstractListModel>

#include "../database/types/dataentity.h"

class MetaModel final : public QAbstractListModel{
Q_OBJECT
public:
    explicit MetaModel(QObject *parent = nullptr);

    void setEntity(DataEntity *node);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool addData(QString name);

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool removeMeta(int row);

    void clean();

    [[nodiscard]] bool isActive() const;

private:
    DataEntity *entity{};

    QWidget *view{};
};



#endif //METAMODEL_H
