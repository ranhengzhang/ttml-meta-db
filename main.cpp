#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("TTML元数据管理");
    w.setWindowIcon(QIcon(":/res/favicon.ico"));
    w.show();
    return a.exec();
}
