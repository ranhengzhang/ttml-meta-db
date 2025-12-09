//
// Created by LEGION on 25-3-10.
//

#include "idmodel.h"

#include <QWidget>
#include <QEventLoop>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QSettings>
#include <QTimer>

QList<QString> IDModel::id_options = {
    "ncmMusicId",
    "qqMusicId",
    "spotifyId",
    "appleMusicId",
    "isrc"
};

IDModel *IDModel::emitter = new IDModel(nullptr);

IDModel::IDModel(QObject *parent) {
    view = dynamic_cast<QTableView *>(parent);
}

int IDModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return ids == nullptr ? 0 : ids->size();
}

int IDModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

QVariant IDModel::data(const QModelIndex &index, int role) const {
    if (ids == nullptr || !index.isValid())
        return {};

    // 明确阻止检查状态显示
    if (role == Qt::CheckStateRole)
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return ids->at(index.row())[index.column()];
    return {};
}

bool IDModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (index.column() == 0) {
        if (!id_options.contains(value.toString()))
            return false;
        (*ids)[index.row()][0] = value.toString();
    } else {
        bool ok = false;
        const auto id = analyseId((*ids)[index.row()][0], value.toString(), &ok);
        if (!ok) {
            return false;
        }
        (*ids)[index.row()][1] = id;
        view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    }
    emit dataChanged(index, index);
    return true;
}

bool IDModel::addData(const QString &key, const QString &value) {
    if (ids->contains({key, value})) return false;

    beginInsertRows(QModelIndex(), ids->size(), ids->size());
    const QList val{key, value};

    ids->append(val);
    endInsertRows();
    return true; // 插入成功
}

Qt::ItemFlags IDModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == 0 || index.column() == 1) {
        flags |= Qt::ItemIsEditable;
        flags &= ~Qt::ItemIsUserCheckable; // 强制移除复选框标志
    }
    return flags;
}

bool IDModel::insertRows(const int row, const int count, const QModelIndex &parent) {
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        ids->insert(row, {id_options.first(), ""}); // 默认值
    }
    endInsertRows();
    return true;
}

bool IDModel::removeRows(const int row, const int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        ids->removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool IDModel::isActive() const {
    return ids != nullptr;
}

/**
 * 设置当前单曲的 ID 列表和 UUID
 * @param id_list ID 列表
 * @param track_uuid 单曲 UUID
 */
void IDModel::setFamily(QList<QList<QString> > *id_list, const QString &track_uuid) {
    beginResetModel();
    ids = id_list;
    this->track_uuid = track_uuid;
    endResetModel();
}

void IDModel::clean() {
    beginResetModel();
    ids = {};
    track_uuid = {};
    endResetModel();
}

QStringList IDModel::options() {
    return id_options;
}

QVariant IDModel::headerData(const int section, const Qt::Orientation orientation, const int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Key";
            case 1: return "Value";
            default: return {};
        }
    }
    return {};
}

/**
 * 对于传入的 ID 或 URL 进行分析提取
 * @param key ID 类型
 * @param value ID 或 URL
 * @param ok 返回 ID 或 URL 是否合法
 * @return 分析得到的 ID
 */
QString IDModel::analyseId(const QString &key, QString value, bool *ok) {
    *ok = true;
    if (key == "ncmMusicId") {
        const QRegularExpression primary(R"(^[0-9]+$)");
        const auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            // 不是纯 ID
            if (value.contains("163cn.tv")) {
                // 移动端链接
                QNetworkAccessManager manager;
                const auto request = QNetworkRequest(QUrl(value));
                QNetworkReply *reply = manager.get(request);

                // 阻塞等待请求完成
                QEventLoop loop;
                QTimer timer;
                timer.setSingleShot(true);

                // 连接信号（完成/超时）
                connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
                connect(&timer, &QTimer::timeout, [&]() {
                    loop.quit();
                    reply->abort(); // 主动终止请求
                });

                timer.start(10 * 1000);
                loop.exec();

                // 判断超时情况
                const bool isTimeout = !timer.isActive();
                if (isTimeout) {
                    reply->deleteLater();
                    *ok = false;
                    return {};
                }

                // 正常错误处理
                if (reply->error() != QNetworkReply::NoError) {
                    reply->deleteLater();
                    *ok = false;
                    return {};
                }

                // 处理内容...
                const QString content = QString::fromUtf8(reply->readAll());
                reply->deleteLater();

                // 查找目标字符串
                const auto targetIndex = content.indexOf(R"(meta property="og:url")");
                if (targetIndex == -1) {
                    *ok = false;
                    return {};
                }

                // 正则匹配
                const QRegularExpression extract(R"((?<=content=\").*?(?=\"))");
                const auto href = extract.match(content, targetIndex);
                if (!href.hasMatch()) {
                    *ok = false;
                    return {};
                }
                value = href.captured(0); // 使用正确链接覆盖
            }

            // 提取 ID
            const QRegularExpression extract(R"((?<=id\=)[0-9]+)");
            const auto match = extract.match(value);
            if (!match.hasMatch()) {
                *ok = false;
                return {};
            }

            // 尝试获取别名
            QNetworkAccessManager manager;
            const auto request = QNetworkRequest(QUrl(value));
            QNetworkReply *reply = manager.get(request);

            // 阻塞等待请求完成
            QEventLoop loop;
            QTimer timer;
            timer.setSingleShot(true);

            // 连接信号（完成/超时）
            connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            connect(&timer, &QTimer::timeout, [&]() {
                loop.quit();
                reply->abort(); // 主动终止请求
            });

            timer.start(10 * 1000);
            loop.exec();

            // 判断超时情况
            const bool isTimeout = !timer.isActive();
            if (isTimeout) {
                reply->deleteLater();
                *ok = false;
                return {};
            }

            // 正常错误处理
            if (reply->error() != QNetworkReply::NoError) {
                reply->deleteLater();
                *ok = false;
                return {};
            }

            // 处理内容...
            const QString content = QString::fromUtf8(reply->readAll());
            reply->deleteLater();

            // 查找目标字符串
            const auto targetIndex = content.indexOf(R"(subtit)");
            if (targetIndex >= 0) {
                // 正则匹配
                const QRegularExpression find(R"((?<=\"\>\n).*?(?=\n))");
                const auto subtitle = find.match(content, targetIndex);
                if (subtitle.hasMatch() && !subtitle.captured(0).isEmpty()) {
                    emit emitter->subtitleGot(subtitle.captured(0));
                }
            }

            value = match.captured(0);
        }
    } else if (key == "spotifyId") {
        const QRegularExpression primary(R"(^[0-9a-zA-Z]+$)");
        const auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            // 不是纯 ID
            const QRegularExpression extract(R"((?<=track[/:])[0-9a-zA-Z]+)");
            const auto match = extract.match(value);
            if (!match.hasMatch()) {
                *ok = false;
                return {};
            }
            value = match.captured(0);
        }
    } else if (key == "appleMusicId") {
        const QRegularExpression primary(R"(^[0-9]+$)");
        const auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            // 不是纯 ID
            const QRegularExpression application(R"((?<=i\=)[0-9]+)");
            auto match = application.match(value);
            if (!match.hasMatch()) {
                const QRegularExpression website(R"(([0-9]+)($|\?))");
                match = website.match(value);
                if (!match.hasMatch()) {
                    *ok = false;
                    return {};
                }
                value = match.captured(1);
            } else {
                value = match.captured(0);
            }
        }
    } else if (key == "qqMusicId") {
        const QRegularExpression primary(R"(^[0-9a-zA-Z]+$)");
        const auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            // 不是纯 ID
            const QRegularExpression mobile(R"(u\?__=[0-9a-zA-Z])");
            const auto need = mobile.match(value);
            if (need.hasMatch()) {
                // 客户端链接
                QNetworkAccessManager manager;
                auto request = QNetworkRequest(QUrl(value));
                QNetworkReply *reply = manager.get(request);

                // 阻塞等待请求完成
                QEventLoop loop;
                QTimer timer;
                timer.setSingleShot(true);

                // 连接信号（完成/超时）
                connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
                connect(&timer, &QTimer::timeout, [&]() {
                    loop.quit();
                    reply->abort(); // 主动终止请求
                });

                timer.start(10 * 1000);
                loop.exec();

                // 判断超时情况
                bool isTimeout = !timer.isActive();
                if (isTimeout) {
                    reply->deleteLater();
                    *ok = false;
                    return {};
                }

                // 正常错误处理
                if (reply->error() != QNetworkReply::NoError) {
                    reply->deleteLater();
                    *ok = false;
                    return {};
                }

                // 处理内容...
                QString content = QString::fromUtf8(reply->readAll());
                reply->deleteLater();

                // 查找目标字符串
                auto targetIndex = content.indexOf(R"("songList")");
                if (targetIndex == -1) {
                    targetIndex = content.indexOf(R"(property="og:url" content=")");

                    if (targetIndex == -1) {
                        *ok = false;
                        return {};
                    }

                    value = content.mid(targetIndex, content.indexOf(R"("/>)", targetIndex + 27) - targetIndex).mid(27);
                    return analyseId(key, value, ok);
                }

                // 正则匹配
                const QRegularExpression re(R"((?<=\"mid\":\")[0-9A-Za-z]+)");
                const auto match = re.match(content, targetIndex);

                if (!match.hasMatch()) {
                    *ok = false;
                    return {};
                }
                value = match.captured(0);

                // 尝试获取别名
                const QRegularExpression ti(R"((?<=\"subtitle\":\").*?(?=\"))");
                const auto exists = ti.match(content, targetIndex);

                if (exists.hasMatch() && !exists.captured(0).isEmpty()) {
                    emit IDModel::emitter->subtitleGot(exists.captured(0));
                }
            } else {
                // 网页端链接(可以直接获取到 ID)
                const QRegularExpression extract(R"((songDetail/|songmid=)([0-9a-zA-Z]+))");
                const auto mid = extract.match(value);
                if (!mid.hasMatch()) {
                    *ok = false;
                    return {};
                }
                value = mid.captured(2);
            }
        }


        QSettings config{"settings.ini", QSettings::IniFormat};
        if (config.contains("qqMusicCookie")) {
            QNetworkAccessManager manager;
            auto request = QNetworkRequest(QUrl(QString(R"(https://y.qq.com/n/ryqq_v2/songDetail/)") + value));

            request.setRawHeader("Accept", "*/*");
            //                    request.setRawHeader("Accept-Encoding", "gzip, deflate, br");
            request.setRawHeader("Connection", "keep-alive");
            request.setRawHeader("Cookie", config.value("qqMusicCookie").toByteArray());
            request.setRawHeader("User-Agent",
                                 R"(Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/142.0.0.0 Safari/537.36)");

            QNetworkReply *reply = manager.get(request);

            // 阻塞等待请求完成
            QEventLoop loop;
            QTimer timer;
            timer.setSingleShot(true);

            // 连接信号（完成/超时）
            connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            connect(&timer, &QTimer::timeout, [&]() {
                loop.quit();
                reply->abort(); // 主动终止请求
            });

            timer.start(10 * 1000);
            loop.exec();

            // 判断超时情况
            bool isTimeout = !timer.isActive();
            if (isTimeout) {
                reply->deleteLater();
                return value;
            }

            // 正常错误处理
            if (reply->error() != QNetworkReply::NoError) {
                reply->deleteLater();
                return value;
            }

            // 1. 创建正则：匹配冒号(:)，跟随任意空白(\s*)，然后是undefined，最后是单词边界(\b)
            // \b 确保不会匹配到 "undefinedVariable" 这样的词
            QRegularExpression re(R"((:\s*)undefined\b)");

            // 处理内容...
            // 2. 替换为 null
            // \1 代表保留捕获组1的内容（即冒号和空格）
            QString content = QString::fromUtf8(reply->readAll()).replace(re, "\\1null");
            reply->deleteLater();

            // if content match /(?<=window.__INITIAL_DATA__ =).*?(?=</script>)/
            if (content.contains(R"(window.__INITIAL_DATA__ =)")) {
                //                        qDebug() << content;
                const auto begin = content.indexOf(R"(window.__INITIAL_DATA__ =)") + 25;
                const auto end = content.indexOf(R"(</script>)", begin);
                if (begin != -1 and end != -1 and begin < end) {
                    const auto data = content.mid(begin, end - begin);
                    qDebug().noquote() << data;
                    // to json
                    const auto json = QJsonDocument::fromJson(data.toUtf8());
                    if (json.isObject()) {
                        const auto obj = json.object();
                        const auto songList = obj["songList"];
                        if (songList.isArray() and songList.toArray().size() > 0) {
                            const auto song = songList.toArray()[0].toObject();
                            auto mid = song["mid"].toString();
                            if (!mid.isEmpty() and mid != value) {
                                emit emitter->idGot(IDModel::id_options[1], mid);
                            }
                            auto id = QString::number(song["id"].toInteger(-1));
                            if (id != "-1" and id != value) {
                                emit emitter->idGot(IDModel::id_options[1], id);
                            }
                            auto title = song["title"].toString();
                            if (!title.isEmpty()) {
                                emit emitter->subtitleGot(title);
                            }
                            auto subtitle = song["subtitle"].toString();
                            if (!subtitle.isEmpty()) {
                                emit emitter->subtitleGot(subtitle);
                            }
                        }
                    }
                }
            }
        }
    }
    return value;
}
