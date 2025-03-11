//
// Created by LEGION on 25-3-10.
//

#ifndef IDDELEGATE_H
#define IDDELEGATE_H

#include <QStyledItemDelegate>

class IDDelegate: public QStyledItemDelegate {
Q_OBJECT
public:
    IDDelegate(const QStringList &options, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                     const QModelIndex &index) const override;

private:
    QStringList id_options;
};



#endif //IDDELEGATE_H
