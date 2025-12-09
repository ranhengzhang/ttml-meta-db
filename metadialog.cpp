#include "metadialog.h"
#include "ui_metadialog.h"

MetaDialog::MetaDialog(QWidget *parent) : QDialog(parent),
                                          ui(new Ui::MetaDialog) {
    ui->setupUi(this);

    ui->key_text->setFocus();
}

MetaDialog::~MetaDialog() {
    delete ui;
}

QString MetaDialog::getKeyText() const {
    return ui->key_text->text();
}

QString MetaDialog::getValueText() const {
    return ui->value_text->text();
}
