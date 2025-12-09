#ifndef METADIALOG_H
#define METADIALOG_H

#include <QDialog>

namespace Ui {
class MetaDialog;
}

class MetaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MetaDialog(QWidget *parent = nullptr);

    ~MetaDialog();

    [[nodiscard]] QString getKeyText() const;

    [[nodiscard]] QString getValueText() const;

private:
    Ui::MetaDialog *ui;
};

#endif // METADIALOG_H
