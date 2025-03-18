#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "AlbumArtistModel.h"
#include "albummodel.h"
#include "artistmodel.h"
#include "idmodel.h"
#include "metamodel.h"
#include "track.h"
#include "trackalbummodel.h"
#include "trackfeatmodel.h"
#include "trackmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_splitter_4_splitterMoved(int pos, int index);

    void on_splitter_6_splitterMoved(int pos, int index);

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_tabWidget_currentChanged(int index) const;

    void on_artists_list_clicked(const QModelIndex &index) const;

    void on_albums_list_clicked(const QModelIndex &index) const;

    void on_tracks_list_clicked(const QModelIndex &index) const;

    void on_artists_list_customContextMenuRequested(const QPoint &pos);

    void on_artist_meta_customContextMenuRequested(const QPoint &pos);

    void on_albums_list_customContextMenuRequested(const QPoint &pos);

    void on_album_meta_customContextMenuRequested(const QPoint &pos);

    void on_album_artists_customContextMenuRequested(const QPoint &pos);

    void on_tracks_list_customContextMenuRequested(const QPoint &pos);

    void on_track_meta_customContextMenuRequested(const QPoint &pos);

    void on_track_albums_customContextMenuRequested(const QPoint &pos);

    void on_track_feats_customContextMenuRequested(const QPoint &pos);

    void on_track_id_customContextMenuRequested(const QPoint &pos);

    void onAddArtist();

    void onDeleteArtist();

    void onAddArtistMeta();

    void onDeleteArtistMeta();

    void onAddNewAlbum();

    void onAddOldAlbum();

    void onDeleteAlbum();

    void onAddAlbumMeta();

    void onDeleteAlbumMeta();

    void onAddAlbumToArtist();

    void onDeleteAlbumFromArtist();

    void onAddNewTrack();

    void onAddOldTrack();

    void onDeleteTrack();

    void onAddTrackMeta();

    void onDeleteTrackMeta();

    void onAddTrackToAlbum();

    void onDeleteTrackFromAlbum();

    void onAddTrackFeat();

    void onDeleteTrackFeat();

    void onAddID();

    void on_copy_meta_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QString filePath{};
    static constexpr int HOTKEY_ID = 1;
    HWND hWnd{};

    void saveFile() const;

    void showToast(const QString &name);

    bool moved = false;

    int pendingRow = -1;

    QTimer *timer{};

    ArtistModel* artist_list_model{};

    AlbumModel* album_list_model{};

    TrackModel* track_list_model{};

    MetaModel* artist_meta_model{};

    MetaModel* album_meta_model{};

    MetaModel* track_meta_model{};

    AlbumArtistModel* album_artist_model{};

    TrackAlbumModel* track_album_model{};

    TrackFeatModel* track_feat_model{};

    IDModel* id_model{};
};
#endif // MAINWINDOW_H
