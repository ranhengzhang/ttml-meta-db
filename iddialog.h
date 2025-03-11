#ifndef IDDIALOG_H
#define IDDIALOG_H

#include <QDialog>

namespace Ui {
class IDDialog;
}

class IDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IDDialog(QWidget *parent = nullptr);
    ~IDDialog();

    QString getSelectedOption() const;

    QString getInputText() const;

private:
    Ui::IDDialog *ui;
};

#endif // IDDIALOG_H
