#include "iddialog.h"
#include "ui_iddialog.h"

IDDialog::IDDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IDDialog)
{
    ui->setupUi(this);

    ui->lineEdit->setFocus();
}

IDDialog::~IDDialog()
{
    delete ui;
}
// 获取返回的下拉框选项和值
QString IDDialog::getSelectedOption() const {
    return ui->comboBox->currentText();
}

QString IDDialog::getInputText() const {
    return ui->lineEdit->text();
}
