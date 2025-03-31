//
// Created by LEGION on 25-3-10.
//

#ifndef IDMODEL_H
#define IDMODEL_H

#include <QTableView>


class IDModel: public QAbstractTableModel {
Q_OBJECT
public:
    explicit IDModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool addData(const QString& key, const QString& value);

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent) override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    [[nodiscard]] bool isActive() const;

    void setFamily(QList<QList<QString>> *id_list, const QString& track_uuid);

    void clean();

    static QStringList options();

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] static QString analyseId(const QString &key, QString value, bool *ok);

    static QStringList id_options;

    static IDModel *emitter;

signals:
    void subtitleGot(QString subtitle);

private:
    QList<QList<QString>> *ids{};

    QString track_uuid{};

    QTableView *view{};
};



#endif //IDMODEL_H
