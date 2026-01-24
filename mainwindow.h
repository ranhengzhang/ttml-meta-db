#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "albumartistmodel.h"
#include "albummodel.h"
#include "artistmembermodel.h"
#include "artistmodel.h"
#include "extramodel.h"
#include "idmodel.h"
#include "metamodel.h"
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
    void on_splitter_7_splitterMoved(int pos, int index);

    void on_splitter_6_splitterMoved(int pos, int index);

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionCookie_triggered();

    void on_tabWidget_currentChanged(int index) const;

    void on_artists_list_clicked(const QModelIndex &index) const;

    void on_albums_list_clicked(const QModelIndex &index) const;

    void on_tracks_list_clicked(const QModelIndex &index) const;

    void on_artists_list_customContextMenuRequested(const QPoint &pos);

    void on_artist_members_customContextMenuRequested(const QPoint &pos);

    void on_artist_meta_customContextMenuRequested(const QPoint &pos);

    void on_albums_list_customContextMenuRequested(const QPoint &pos);

    void on_album_meta_customContextMenuRequested(const QPoint &pos);

    void on_album_artists_customContextMenuRequested(const QPoint &pos);

    void on_tracks_list_customContextMenuRequested(const QPoint &pos);

    void on_track_meta_customContextMenuRequested(const QPoint &pos);

    void on_track_albums_customContextMenuRequested(const QPoint &pos);

    void on_track_feats_customContextMenuRequested(const QPoint &pos);

    void on_track_id_customContextMenuRequested(const QPoint &pos);

    void on_extra_meta_customContextMenuRequested(const QPoint &pos);

    void onAddArtist();

    void onDeleteArtist();

    void onAddArtistMeta();

    void onDeleteArtistMeta();

    void onAddArtistMember();

    void onDeleteArtistMember();

    void onAddNewAlbum();

    void onAddOldAlbum();

    void onDeleteAlbum();

    void onAddAlbumMeta();

    void onDeleteAlbumMeta();

    void onAddAlbumToArtist();

    void onDeleteAlbumFromArtist();

    void onAddNewTrack();

    void onAddOldTrack();

    void onMoveUpTrack();

    void onMoveDownTrack();

    void onDeleteTrack();

    void onAddTrackMeta();

    void onDeleteTrackMeta();

    void onAddTrackToAlbum();

    void onDeleteTrackFromAlbum();

    void onAddTrackFeat();

    void onDeleteTrackFeat();

    void onAddID();

    void onAddExtra();

    void on_copy_meta_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QString _file_path{};
    static constexpr int HOTKEY_ID = 1;
    HWND _h_wnd{};
    QSettings _config{"settings.ini", QSettings::IniFormat};

    void openFile(const QString &open_file_path);

    void saveFile();

    void showToast(const QString &name);

    bool _moved = false;

    int _pending_row = -1;

    QTimer *_timer{};

    ArtistModel* _artist_list_model{};

    AlbumModel* _album_list_model{};

    TrackModel* _track_list_model{};

    MetaModel* _artist_meta_model{};

    MetaModel* _album_meta_model{};

    MetaModel* _track_meta_model{};

    ArtistMemberModel* _artist_member_model{};

    AlbumArtistModel* _album_artist_model{};

    TrackAlbumModel* _track_album_model{};

    TrackFeatModel* _track_feat_model{};

    IDModel* _id_model{};

    ExtraModel* _extra_model{};
};
#endif // MAINWINDOW_H
