//
// Created by LEGION on 25-3-10.
//

#ifndef IDMODEL_H
#define IDMODEL_H

#include "AlbumArtistModel.h"

class IDModel: public QAbstractTableModel {
Q_OBJECT
public:
    explicit IDModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool addData(QString key, QString value);

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent) override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    [[nodiscard]] bool isActive() const;

    void setFamily(QList<QList<QString>> *list, const QString& uuid);

    void clean();

    static QStringList options();

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] static QString analyseId(QString key, QString value, bool *ok);

private:
    QList<QList<QString>> *ids{};

    QString track_uuid{};

    static QStringList id_options;

    QWidget *view{};

    void refreshAll();
};



#endif //IDMODEL_H
