#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QInputDialog>
#include <QClipboard>
#include <QTimer>
#include <QShortcut>
#include <QCloseEvent>

#include "albummodel.h"
#include "artistmodel.h"
#include "database.h"
#include "metamodel.h"
#include "AlbumArtistModel.h"
#include "iddelegate.h"
#include "iddialog.h"
#include "idmodel.h"
#include "trackalbummodel.h"
#include "trackfeatmodel.h"
#include "trackmodel.h"
#include "wintoast.hpp"
#include "wintoastlib.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    ui->artists_list->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->artist_meta->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->albums_list->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->album_meta->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->album_artists->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tracks_list->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->track_meta->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->track_albums->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->track_feats->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->track_id->setContextMenuPolicy(Qt::CustomContextMenu);

    artist_list_model = new ArtistModel(ui->artists_list);
    album_list_model = new AlbumModel(ui->albums_list);
    track_list_model = new TrackModel(ui->tracks_list);
    artist_meta_model = new MetaModel(ui->artist_meta);
    album_meta_model = new MetaModel(ui->album_meta);
    track_meta_model = new MetaModel(ui->track_meta);
    album_artist_model = new AlbumArtistModel(ui->album_artists);
    track_album_model = new TrackAlbumModel(ui->track_albums);
    track_feat_model = new TrackFeatModel(ui->track_feats);
    id_model = new IDModel(ui->track_id);

    ui->artists_list->setModel(artist_list_model);
    ui->albums_list->setModel(album_list_model);
    ui->tracks_list->setModel(track_list_model);
    ui->artist_meta->setModel(artist_meta_model);
    ui->album_meta->setModel(album_meta_model);
    ui->track_meta->setModel(track_meta_model);
    ui->album_artists->setModel(album_artist_model);
    ui->track_albums->setModel(track_album_model);
    ui->track_feats->setModel(track_feat_model);
    ui->track_id->setModel(id_model);

    timer = new QTimer(this);
    timer->setInterval(300000);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!filePath.isEmpty()) {
            saveFile();
        }
    });
    timer->start();

    WinToast::instance()->setAppName(L"TTML META DB");
    WinToast::instance()->setAppUserModelId(WinToast::configureAUMI(L"TTML META DB", L"TTML.META.DB"));
    if (!WinToast::instance()->initialize()) {
        ui->statusbar->showMessage("wintoast未正确初始化");
        qDebug() << "Error, your system in not compatible!";
    }

    auto *shortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(shortcut,&QShortcut::activated ,this,&MainWindow::on_actionSave_triggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_splitter_4_splitterMoved(int pos, int index) {
    if (moved) return;
    moved = true;

    // 计算splitterB的比例并应用到splitterA
    QList<int> sizes4 = ui->splitter_4->sizes();
    int total4 = ui->splitter_4->width();
    if (total4 > 0) {
        int total6 = ui->splitter_6->width();
        QList<int> newSizes6;
        for (int size : sizes4) {
            newSizes6.append(size * total6 / total4);
        }
        ui->splitter_6->setSizes(newSizes6);
    }

    moved = false;
}

void MainWindow::on_splitter_6_splitterMoved(int pos, int index) {
    if (moved) return;
    moved = true;

    // 计算splitterB的比例并应用到splitterA
    QList<int> sizes6 = ui->splitter_6->sizes();
    int total6 = ui->splitter_6->width();
    if (total6 > 0) {
        int total4 = ui->splitter_4->width();
        QList<int> newSizes4;
        for (int size : sizes6) {
            newSizes4.append(size * total4 / total6);
        }
        ui->splitter_4->setSizes(newSizes4);
    }

    moved = false;
}


void MainWindow::on_actionOpen_triggered()
{
    const QString openPath = QFileDialog::getOpenFileName(this, "打开元数据库文件", "", "TTML元数据库 (*.metadb)");

    if (openPath.isEmpty()) {
        QMessageBox::information(this, "", "用户取消打开文件", QMessageBox::Ok);
        return;
    } else {
        filePath = openPath;
        // 打开文件并读取内容
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open file!";
            return;
        }

        // 读取压缩文件内容
        const QByteArray compressedData = file.readAll();
        file.close();

        // 使用 QZlibCodec 解压 gzip 数据
        const QByteArray decompressedData = qUncompress(compressedData);

        // 尝试解析 JSON 数据
        const QJsonDocument document = QJsonDocument::fromJson(decompressedData);
        // const QJsonDocument document = QJsonDocument::fromJson(compressedData);

        if (document.isNull()) {
            qDebug() << "Failed to parse JSON.";
            return;
        }

        // 获取 JSON 对象
        if (!document.isObject()) {
            qDebug() << "The JSON is not an object.";
            return;
        }

        // 获取 JSON 对象
        if (document.isObject()) {
            QJsonObject jsonObject = document.object();

            // 处理第一个数组
            DataBase::artists.clear();
            if (jsonObject.contains("artists") && jsonObject["artists"].isArray()) {
                QJsonArray artists_json = jsonObject["artists"].toArray();
                for (const QJsonValue artist : artists_json) {
                    const QJsonObject item = artist.toObject();
                    Artist new_artist(item);
                    DataBase::artists.insert(new_artist.getUUID(), new_artist);
                }
            }

            // 处理第二个数组
            DataBase::albums.clear();
            if (jsonObject.contains("albums") && jsonObject["albums"].isArray()) {
                QJsonArray albums_json = jsonObject["albums"].toArray();
                for (const QJsonValue value : albums_json) {
                    const QJsonObject item = value.toObject();
                    Album new_album(item);
                    DataBase::albums.insert(new_album.getUUID(), new_album);
                }
            }

            // 处理第三个数组
            DataBase::tracks.clear();
            if (jsonObject.contains("tracks") && jsonObject["tracks"].isArray()) {
                QJsonArray tracks_json = jsonObject["tracks"].toArray();
                for (const QJsonValue value : tracks_json) {
                    const QJsonObject item = value.toObject();
                    Track new_track(item);
                    DataBase::tracks.insert(new_track.getUUID(), new_track);
                }
            }

            artist_list_model->refreshAll();

            ui->statusbar->showMessage("导入成功");
            ui->tabWidget->setCurrentIndex(0);
        } else {
            qDebug() << "The JSON is not an object.";
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (filePath.isEmpty()) {
        on_actionSaveAs_triggered();
    } else {
        saveFile();
    }
}


void MainWindow::on_actionSaveAs_triggered()
{
    // 弹出文件对话框选择保存路径
    const QString savePath = QFileDialog::getSaveFileName(this,"保存元数据库文件","",  "TTML元数据库 (*.metadb)"
    );

    if (savePath.isEmpty()) {
        QMessageBox::information(this, "", "用户取消保存文件", QMessageBox::Ok);
        qDebug() << "No file selected.";
        return;
    }

    filePath = savePath;
    saveFile();
}

void MainWindow::on_tabWidget_currentChanged(const int index) const {
    if (index == 1) {
        auto selected_artist = ui->artists_list->selectionModel()->selectedIndexes();
        if (selected_artist.isEmpty()) {
            album_list_model->clean();
            album_artist_model->clean();
            album_meta_model->clean();
        } else {
            const auto artist_uuid = artist_list_model->getArtistByRow(selected_artist.first().row());
            if (artist_uuid != album_list_model->getArtist()) {
                album_list_model->setArtist(artist_uuid);
                album_artist_model->clean();
                album_meta_model->clean();
            }
        }
    } else if (index == 2) {
        auto selected_album = ui->albums_list->selectionModel()->selectedIndexes();
        if (selected_album.isEmpty()) {
            track_list_model->clean();
            track_album_model->clean();
            track_feat_model->clean();
            track_meta_model->clean();
            id_model->clean();
        } else {
            const auto album_uuid = album_list_model->getAlbumByRow(selected_album.first().row());
            if (album_uuid != track_list_model->getAlbum()) {
                track_list_model->setAlbum(album_uuid);
                track_album_model->clean();
                track_feat_model->clean();
                track_meta_model->clean();
                id_model->clean();
            }
        }
    }
}

void MainWindow::on_artists_list_clicked(const QModelIndex &index) const {
    if (!index.isValid()) return;

    // 获取模型和行号
    const int row = index.row();

    // 获取歌手UUID
    const auto artist_uuid = artist_list_model->getArtistByRow(row);

    // 刷新歌手名列表
    artist_meta_model->setEntity(&DataBase::artists[artist_uuid]);
}

void MainWindow::on_albums_list_clicked(const QModelIndex &index) const {
    if (!index.isValid()) return;

    // 获取模型和行号
    const int row = index.row();

    // 获取专辑UUID
    const auto album_uuid = album_list_model->getAlbumByRow(row);

    // 刷新专辑名列表
    album_meta_model->setEntity(&DataBase::albums[album_uuid]);

    // 刷新歌曲列表
    track_list_model->setAlbum(album_uuid);

    // 刷新专辑歌手列表
    album_artist_model->setFamily(&DataBase::albums[album_uuid].artists, album_uuid);
}

void MainWindow::on_tracks_list_clicked(const QModelIndex &index) {
    if (!index.isValid()) return;

    // 获取模型和行号
    const int row = index.row();

    // 获取专辑UUID
    const auto track_uuid = track_list_model->getTrackByRow(row);

    // 刷新专辑名列表
    track_meta_model->setEntity(&DataBase::tracks[track_uuid]);

    // 刷新专辑歌手列表
    track_album_model->setFamily(&DataBase::tracks[track_uuid].albums, track_uuid);

     // 刷新合作歌手列表
    track_feat_model->setFamily(&DataBase::tracks[track_uuid].feats, track_uuid);

    // 刷新ID列表
    id_model->setFamily(&DataBase::tracks[track_uuid].ids, track_uuid);
}

void MainWindow::on_artists_list_customContextMenuRequested(const QPoint &pos) {
    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->artists_list->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->artists_list->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除歌手");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteArtist);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加歌手");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddArtist);

    menu.exec(ui->artists_list->viewport()->mapToGlobal(pos));
}

void MainWindow::on_artist_meta_customContextMenuRequested(const QPoint &pos) {
    if (!ui->artist_meta->model()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->artist_meta->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->artist_meta->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除歌手名");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteArtistMeta);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加歌手名");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddArtistMeta);

    menu.exec(ui->artist_meta->viewport()->mapToGlobal(pos));
}

void MainWindow::on_albums_list_customContextMenuRequested(const QPoint &pos) {
    if (!ui->albums_list->model()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->albums_list->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->albums_list->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除专辑");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteAlbum);
    }

    // 始终显示添加选项
    const QAction *addNewAction = menu.addAction("添加新专辑");
    connect(addNewAction, &QAction::triggered, this, &MainWindow::onAddNewAlbum);

    // 如果有其他歌手专辑显示添加选项
    if (!DataBase::albums.isEmpty()) {
        const QAction *addOldAction = menu.addAction("添加已有专辑");
        connect(addOldAction, &QAction::triggered, this, &MainWindow::onAddOldAlbum);
    }

    menu.exec(ui->albums_list->viewport()->mapToGlobal(pos));
}

void MainWindow::on_album_meta_customContextMenuRequested(const QPoint &pos) {
    auto *model = qobject_cast<MetaModel*>(ui->album_meta->model());
    if (!model || !model->isActive()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->album_meta->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->album_meta->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除专辑名");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteAlbumMeta);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加专辑名");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddAlbumMeta);

    menu.exec(ui->album_meta->viewport()->mapToGlobal(pos));
}

void MainWindow::on_album_artists_customContextMenuRequested(const QPoint &pos) {
    auto *model = qobject_cast<AlbumArtistModel*>(ui->album_artists->model());
    if (!model || !model->isActive()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->album_artists->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->album_artists->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("从歌手中删除");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteAlbumFromArtist);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加专辑歌手");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddAlbumToArtist);

    menu.exec(ui->album_artists->viewport()->mapToGlobal(pos));
}

void MainWindow::on_tracks_list_customContextMenuRequested(const QPoint &pos) {
    auto *model = qobject_cast<TrackModel*>(ui->tracks_list->model());
    if (!model || !model->isActive()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->tracks_list->indexAt(pos);
    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->tracks_list->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除单曲");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTrack);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加新单曲");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddNewTrack);

    // 如果有其他单曲显示添加选项
    if (!DataBase::tracks.isEmpty()) {
        const QAction *addOldAction = menu.addAction("添加已有单曲");
        connect(addOldAction, &QAction::triggered, this, &MainWindow::onAddOldTrack);
    }

    menu.exec(ui->tracks_list->viewport()->mapToGlobal(pos));
}

void MainWindow::on_track_meta_customContextMenuRequested(const QPoint &pos) {
    auto *model = qobject_cast<MetaModel*>(ui->track_meta->model());
    if (!model || !model->isActive()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->track_meta->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->track_meta->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除单曲名");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTrackMeta);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加单曲名");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddTrackMeta);

    menu.exec(ui->track_meta->viewport()->mapToGlobal(pos));
}

void MainWindow::on_track_albums_customContextMenuRequested(const QPoint &pos) {
    auto *model = qobject_cast<TrackAlbumModel*>(ui->track_albums->model());
    if (!model || !model->isActive()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->track_albums->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->track_albums->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("从专辑中删除");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTrackFromAlbum);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加单曲专辑");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddTrackToAlbum);

    menu.exec(ui->track_albums->viewport()->mapToGlobal(pos));
}

void MainWindow::on_track_feats_customContextMenuRequested(const QPoint &pos) {
    auto *model = qobject_cast<TrackFeatModel*>(ui->track_feats->model());
    if (!model || !model->isActive()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->track_feats->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->track_feats->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除参与艺人");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTrackFeat);
    }

    // 始终显示添加选项
    const QAction *addAction = menu.addAction("添加参与艺人");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddTrackFeat);

    menu.exec(ui->track_feats->viewport()->mapToGlobal(pos));
}

void MainWindow::on_track_id_customContextMenuRequested(const QPoint &pos) {
    auto *model = qobject_cast<IDModel*>(ui->track_id->model());
    if (!model || !model->isActive()) {
        return;
    }

    QMenu menu;

    // 获取当前点击位置的索引
    const QModelIndex index = ui->track_id->indexAt(pos);

    // 如果点击的是有效项，添加删除选项
    if (index.isValid()) {
        // 确保右键点击的项被选中
        ui->track_id->selectionModel()->select(index, QItemSelectionModel::Select);

        const QAction *deleteAction = menu.addAction("删除ID");
        connect(deleteAction, &QAction::triggered, this, [this, index, model]() {
            model->removeRow(index.row());
        });
    }

    QAction *addAction = menu.addAction("添加新行");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddID);

    menu.exec(ui->track_id->viewport()->mapToGlobal(pos));
}

// 添加歌手到列表
void MainWindow::onAddArtist() {
    bool ok;
    const QString name = QInputDialog::getText(
        this,
        "添加歌手",
        "请输入名称：",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (ok && !name.isEmpty()) {
        if (!artist_list_model->addData(name)) {
            QMessageBox::warning(this, "错误", "名称已存在或无效！");
        }
    }
}

// 从列表中删除歌手
void MainWindow::onDeleteArtist() {
    // 获取当前选中项
    QModelIndexList selected = ui->artists_list->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        if (artist_list_model) {
            const int row = selected.first().row(); // 获取行号
            artist_list_model->removeArtist(row);
        }
    }
}

// 为选中歌手添加别名
void MainWindow::onAddArtistMeta() {
    bool ok;
    const QString name = QInputDialog::getText(
        this,
        "添加",
        "请输入名称：",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (ok && !name.isEmpty()) {
        if (!artist_meta_model->addData(name)) {
            QMessageBox::warning(this, "错误", "名称已存在或无效！");
        }
    }
}

// 将别名从选中歌手中删除
void MainWindow::onDeleteArtistMeta() {
    // 获取当前选中项
    QModelIndexList selected = ui->artist_meta->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        artist_meta_model->removeMeta(row);
    }
}

// 添加一张新专辑到当前歌手
void MainWindow::onAddNewAlbum() {
    bool ok;
    const QString name = QInputDialog::getText(
        this,
        "添加新专辑",
        "请输入名称：",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (ok && !name.isEmpty()) {
        if (!album_list_model->addNewData(name)) {
            QMessageBox::warning(this, "错误", "名称已存在或无效！");
        }
    }
}

// 添加一张已有专辑到当前歌手
void MainWindow::onAddOldAlbum() {
    auto albums = DataBase::albums.keys();
    QStringList messages{};
    for (auto &album_uuid: albums) {
        auto &album = DataBase::albums[album_uuid];
        QStringList artistNames{};
        for (auto &artist_uuid: album.artists) {
            auto &artist = DataBase::artists[artist_uuid];
            artistNames.append(artist.getName());
        }
        messages.append(QString("%1 - %2 [%3]").arg(album.getName()).arg(artistNames.join('/')).arg(album_uuid));
    }

    bool ok = false;
    QString item = QInputDialog::getItem(
        this,
        "选择已有专辑",
        "专辑名 - 歌手 - 专辑ID",
        messages,
        0,
        false,
        &ok
    );
    if (ok && !item.isEmpty()) {
        if (!album_list_model->addOldData(albums[messages.indexOf(item)])) {
            QMessageBox::warning(this, "错误", "专辑已添加或无效！");
        }
    } else {
        QMessageBox::critical(this, "撤回", "用户取消选择");
    }
}

// 从当前歌手下移除一张专辑
void MainWindow::onDeleteAlbum() {
    // 获取当前选中项
    QModelIndexList selected = ui->albums_list->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        album_list_model->removeAlbumFromArtist(row);
    }
}

void MainWindow::onAddAlbumMeta() {
    bool ok;
    const QString name = QInputDialog::getText(
        this,
        "添加",
        "请输入名称：",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (ok && !name.isEmpty()) {
        if (!album_meta_model->addData(name)) {
            QMessageBox::warning(this, "错误", "名称已存在或无效！");
        }
    }
}

void MainWindow::onDeleteAlbumMeta() {
    // 获取当前选中项
    QModelIndexList selected = ui->album_meta->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        album_meta_model->removeMeta(row);
    }
}

void MainWindow::onAddAlbumToArtist() {
    auto artists = DataBase::artists.keys();
    QStringList messages{};
    for (auto &artist_uuid: artists) {
        auto &artist = DataBase::artists[artist_uuid];
        messages.append(QString("%1 [%2]").arg(artist.getName()).arg(artist_uuid));
    }

    bool ok = false;
    QString item = QInputDialog::getItem(
        this,
        "选择已有歌手",
        "歌手 - 歌手ID",
        messages,
        0,
        false,
        &ok
    );
    if (ok && !item.isEmpty()) {
        if (!album_artist_model->addData(artists[messages.indexOf(item)])) {
            QMessageBox::warning(this, "错误", "歌手已添加或无效！");
        }
    } else {
        QMessageBox::critical(this, "撤回", "用户取消选择");
    }
}

void MainWindow::onDeleteAlbumFromArtist() {
    // 获取当前选中项
    QModelIndexList selected = ui->album_artists->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        album_artist_model->removeData(row);
        album_list_model->refreshAll();
    }
}

void MainWindow::onAddNewTrack() {
    bool ok;
    const QString name = QInputDialog::getText(
        this,
        "添加新单曲",
        "请输入名称：",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (ok && !name.isEmpty()) {
        if (!track_list_model->addNewData(name)) {
            QMessageBox::warning(this, "错误", "名称已存在或无效！");
        }
    }
}

void MainWindow::onAddOldTrack() {
    auto tracks = DataBase::tracks.keys();
    QSet<QString> messages{};
    for (auto &track_uuid: tracks) {
        auto &track = DataBase::tracks[track_uuid];
        QStringList artistNames{};
        for (auto &album_uuid: track.albums) {
            auto &album = DataBase::albums[album_uuid];
            for (auto &artist_uuid: album.artists) {
                auto &artist = DataBase::artists[artist_uuid];
                if (!artistNames.contains(artist.getName()))
                    artistNames.append(artist.getName());
            }
        }
        messages.insert(QString("%1 - %2 [%3]").arg(track.getName()).arg(artistNames.join('/')).arg(track_uuid));
    }

    QStringList messagesList{messages.begin(), messages.end()};
    bool ok = false;
    QString item = QInputDialog::getItem(
        this,
        "选择已有单曲",
        "单曲 - 歌手 - 单曲ID",
        messagesList,
        0,
        false,
        &ok
    );
    if (ok && !item.isEmpty()) {
        QRegularExpression re(R"((?<=\[)[0-9\-]+(?=\]$))");
        auto match = re.match(item);
        if (match.hasMatch() && !track_list_model->addOldData(match.captured(0))) {
            QMessageBox::warning(this, "错误", "单曲已添加或无效！");
        }
    } else {
        QMessageBox::critical(this, "撤回", "用户取消选择");
    }
}

void MainWindow::onDeleteTrack() {
    // 获取当前选中项
    QModelIndexList selected = ui->tracks_list->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        track_list_model->removeTrackFromAlbum(row);
    }
}

void MainWindow::onAddTrackMeta() {
    bool ok;
    const QString name = QInputDialog::getText(
        this,
        "添加",
        "请输入名称：",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (ok && !name.isEmpty()) {
        if (!track_meta_model->addData(name)) {
            QMessageBox::warning(this, "错误", "名称已存在或无效！");
        }
    }
}

void MainWindow::onDeleteTrackMeta() {
    // 获取当前选中项
    QModelIndexList selected = ui->track_meta->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        track_meta_model->removeMeta(row);
    }
}

void MainWindow::onAddTrackToAlbum() {
    QModelIndexList selected = ui->artists_list->selectionModel()->selectedIndexes();
    const auto artist_uuid = artist_list_model->getArtistByRow(selected.first().row());
    auto &artist = DataBase::artists[artist_uuid];
    QStringList messages{};
    for (auto &album_uuid: artist.albums) {
        auto &album = DataBase::albums[album_uuid];
        messages.append(QString("%1 [%2]").arg(album.getName()).arg(album_uuid));
    }

    bool ok = false;
    QString item = QInputDialog::getItem(
        this,
        "选择已有专辑",
        "专辑 - 专辑ID",
        messages,
        0,
        false,
        &ok
    );
    if (ok && !item.isEmpty()) {
        if (!track_album_model->addData(artist.albums[messages.indexOf(item)])) {
            QMessageBox::warning(this, "错误", "专辑已添加或无效！");
        }
    } else {
        QMessageBox::critical(this, "撤回", "用户取消选择");
    }
}

void MainWindow::onDeleteTrackFromAlbum() {
    // 获取当前选中项
    QModelIndexList selected = ui->track_albums->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        track_album_model->removeData(row);
        track_list_model->refreshAll();
    }
}

void MainWindow::onAddTrackFeat() {
    auto artists = DataBase::artists.keys();
    QStringList messages{};
    for (auto &artist_uuid: artists) {
        auto &artist = DataBase::artists[artist_uuid];
        messages.append(QString("%1 [%2]").arg(artist.getName()).arg(artist_uuid));
    }

    bool ok = false;
    QString item = QInputDialog::getItem(
        this,
        "选择已有歌手",
        "歌手 - 歌手ID",
        messages,
        0,
        false,
        &ok
    );
    if (ok && !item.isEmpty()) {
        if (!track_feat_model->addData(artists[messages.indexOf(item)])) {
            QMessageBox::warning(this, "错误", "歌手已添加或无效！");
        }
    } else {
        QMessageBox::critical(this, "撤回", "用户取消选择");
    }
}

void MainWindow::onDeleteTrackFeat() {
    // 获取当前选中项
    QModelIndexList selected = ui->track_feats->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除此项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // 获取模型并删除数据
        const int row = selected.first().row(); // 获取行号
        track_feat_model->removeData(row);
    }
}

void MainWindow::onAddID() {
    IDDialog dialog(this);

    dialog.setWindowTitle("添加ID");
    dialog.setWindowIcon(QIcon(":/res/favicon.ico"));
    if (dialog.exec() == QDialog::Accepted) {
        auto key = dialog.getSelectedOption();
        auto value = dialog.getInputText();
        bool ok = false;
        value = IDModel::analyseId(key, value, &ok);
        if (!ok) {
            QMessageBox::warning(this, "错误", "ID格式或程序错误！");
        }

        if (!id_model->addData(key, value)) {
            QMessageBox::warning(this, "错误", "ID已添加或无效！");
        }
    } else {
        QMessageBox::critical(this, "撤回", "用户取消输入");
    }
}

void MainWindow::on_copy_meta_clicked() {
    if (track_list_model->isActive()) {
        auto selected = ui->tracks_list->selectionModel()->selectedRows();
        auto track_uuid = track_list_model->getTrackByRow(selected.first().row());
        auto &track = DataBase::tracks[track_uuid];
        auto xml = track.toXML().join("");
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(xml);
        showToast(track.getName());
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // 创建询问对话框
    QMessageBox::StandardButton resBtn = QMessageBox::question(
        this,
        "确认退出",
        "退出前需要保存文件吗？",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No  // 默认选中No
        );

    // 根据用户选择处理事件
    if (resBtn == QMessageBox::Yes) {
        if (filePath.isEmpty()) {
            // 弹出文件对话框选择保存路径
            const QString savePath = QFileDialog::getSaveFileName(this,"保存元数据库文件","",  "TTML元数据库 (*.metadb)"
            );

            if (savePath.isEmpty()) {
                QMessageBox::information(this, "", "用户取消保存文件", QMessageBox::Ok);
                qDebug() << "No file selected.";
                event->ignore();
                return;
            }

            filePath = savePath;
        }
        saveFile();
    }
    event->accept();  // 接受关闭事件
}

void MainWindow::showToast(const QString &name) {
    auto templ = WinToastTemplate(WinToastTemplate::Text02);
    // templ.setImagePath(ui->imagePath->text().toStdWString(), static_cast<WinToastTemplate::CropHint>(ui->cropHint->currentData().toInt()));
    // templ.setHeroImagePath(ui->heroPath->text().toStdWString(), ui->inlineHeroImage->isChecked());
    templ.setTextField(name.toStdWString(), WinToastTemplate::FirstLine);
    templ.setTextField(L"元数据已复制到剪贴板", WinToastTemplate::SecondLine);
    // templ.setTextField(ui->secondLine->text().toStdWString(), WinToastTemplate::SecondLine);
    // templ.setTextField(ui->thirdLine->text().toStdWString(), WinToastTemplate::ThirdLine);
    templ.setExpiration(10000);
    // templ.setAudioPath(static_cast<WinToastTemplate::AudioSystemFile>(ui->audioSystemFile->currentData().toInt()));
    // templ.setAudioOption(static_cast<WinToastLib::WinToastTemplate::AudioOption>(ui->audioMode->currentData().toInt()));

    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        QMessageBox::warning(this, "Error", "Could not launch your toast notification!");
    }
}

void MainWindow::saveFile() const {
    if (timer->isActive()) {
        timer->stop();
    }
    timer->start();

    // 打开文件保存压缩数据
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open file for writing!";
        return;
    }

    // 将数组转换为 QJsonArray
    QJsonArray artists_json{};
    for (auto &obj : DataBase::artists) {
        artists_json.append(obj.getSelf());
    }

    QJsonArray albums_json{};
    for (auto &obj : DataBase::albums) {
        albums_json.append(obj.getSelf());
    }

    QJsonArray tracks_json{};
    for (auto &obj : DataBase::tracks) {
        tracks_json.append(obj.getSelf());
    }

    // 将 QJsonArray 封装到 QJsonObject 中
    QJsonObject jsonObject;
    jsonObject["artists"] = artists_json;
    jsonObject["albums"] = albums_json;
    jsonObject["tracks"] = tracks_json;

    // 创建 JSON 文档
    const QJsonDocument doc(jsonObject);

    // 获取 JSON 数据（紧凑格式）
    const QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    // 压缩 JSON 数据
    const QByteArray compressedData = qCompress(jsonData);

    // 写入压缩数据
    file.write(compressedData);
    // file.write(jsonData);
    file.close();

    ui->statusbar->showMessage("导出完成");
}
