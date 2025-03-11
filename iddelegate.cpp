//
// Created by LEGION on 25-3-10.
//

#include "iddelegate.h"

#include <QComboBox>
#include <QLineEdit>

IDDelegate::IDDelegate(const QStringList &options, QObject *parent): QStyledItemDelegate(parent), id_options(options)  {
}

QWidget * IDDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
const QModelIndex &index) const {
    if (index.column() == 0) {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(id_options);
        return editor;
    }
    if (index.column() == 1) {
        QLineEdit *editor = new QLineEdit(parent);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void IDDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.column() == 0) {
        auto *combo = dynamic_cast<QComboBox*>(editor);
        combo->setCurrentText(index.model()->data(index).toString());
    } else if (index.column() == 1) {
        auto *line = dynamic_cast<QLineEdit*>(editor);
        line->setText(index.model()->data(index).toString());
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void IDDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (index.column() == 0) {
        auto *combo = dynamic_cast<QComboBox*>(editor);
        model->setData(index, combo->currentText());
    } else if (index.column() == 1) {
        auto *line = dynamic_cast<QLineEdit*>(editor);
        model->setData(index, line->text());
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
