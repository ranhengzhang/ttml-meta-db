#include "iddialog.h"

#include <QClipboard>

#include "ui_iddialog.h"

QString IDDialog::NCM1 = "music.163.com";

QString IDDialog::NCM2 = "163cn.tv";

QString IDDialog::QQ = "y.qq.com";

QString IDDialog::SPF = "open.spotify.com";

QString IDDialog::AM = "music.apple.com";

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

void IDDialog::on_lineEdit_textChanged(const QString &arg1) const {
    if (arg1.contains(IDDialog::NCM1) || arg1.contains(IDDialog::NCM2)) {
        ui->comboBox->setCurrentText("ncmMusicId");
    } else if (arg1.contains(IDDialog::QQ)) {
        ui->comboBox->setCurrentText("qqMusicId");
    } else if (arg1.contains(IDDialog::SPF)) {
        ui->comboBox->setCurrentText("spotifyMusicId");
    } else if (arg1.contains(IDDialog::AM)) {
        ui->comboBox->setCurrentText("appleMusicId");
    }
}
