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

    ~IDDialog() override;

    [[nodiscard]] QString getSelectedOption() const;

    [[nodiscard]] QString getInputText() const;

private slots:
    void on_lineEdit_textChanged(const QString &arg1) const;

private:
    Ui::IDDialog *ui;

    static QString NCM1;
    static QString NCM2;
    static QString QQ;
    static QString SPF1;
    static QString SPF2;
    static QString AM;
};

#endif // IDDIALOG_H
