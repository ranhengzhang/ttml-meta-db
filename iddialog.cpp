#include "iddialog.h"

#include <QClipboard>

#include "ui_iddialog.h"

IDDialog::IDDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IDDialog)
{
    ui->setupUi(this);

    const QClipboard *clipboard = QApplication::clipboard();
    const QString text = clipboard->text();
    ui->lineEdit->setText(text);
    ui->lineEdit->setFocus();
}

IDDialog::~IDDialog()
{
    delete ui;
}

/**
 * 获取返回的下拉框选项和值
 * @return 当前值(ID 种类)
 */
QString IDDialog::getSelectedOption() const {
    return ui->comboBox->currentText();
}

/**
 * 获取文本框内容
 * @return 输入的 ID 或 URL
 */
QString IDDialog::getInputText() const {
    return ui->lineEdit->text();
}
