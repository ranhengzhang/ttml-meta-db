//
// Created by LEGION on 25-3-10.
//

#include "idmodel.h"

#include <QWidget>
#include <QEventLoop>
#include <QNetworkReply>
#include <QTimer>

QList<QString> IDModel::id_options = {
    "ncmMusicId",
    "qqMusicId",
    "spotifyId",
    "appleMusicId",
    "isrc"
};

IDModel::IDModel(QObject *parent) {
    view = dynamic_cast<QWidget*>(parent);
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

    if (role == Qt::DisplayRole || role ==  Qt::EditRole)
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
        auto id = analyseId((*ids)[index.row()][0], value.toString(), &ok);
        if (!ok) {
            return false;
        }
        (*ids)[index.row()][1] = id;
    }
    emit dataChanged(index, index);
    return true;
}

bool IDModel::addData(QString key, QString value) {
    // bool exist = false;
    // for (auto &item: *ids) {
    //     exist |= item[0] == key && item[1] == value;
    // }
    // if (exist) {
    //     return false;
    // }
    if (ids->contains({key, value})) return false;


    beginInsertRows(QModelIndex(), ids->size(), ids->size());
    QList val{key, value};

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

bool IDModel::insertRows(int row, int count, const QModelIndex &parent) {
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        ids->insert(row, {id_options.first(), ""}); // 默认值
    }
    endInsertRows();
    return true;
}

bool IDModel::removeRows(int row, int count, const QModelIndex &parent) {
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

void IDModel::setFamily(QList<QList<QString>> *list, const QString &uuid) {
    ids = list;
    track_uuid = uuid;

    refreshAll();
}

void IDModel::clean() {
    ids = {};
    track_uuid = {};

    refreshAll();
}

QStringList IDModel::options() {
    return id_options;
}

QVariant IDModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Key";
            case 1: return "Value";
        }
    }
    return QVariant();
}

QString IDModel::analyseId(QString key, QString value, bool *ok) {
    *ok = true;
    if (key == "ncmMusicId") {
        const QRegularExpression primary(R"(^[0-9]+$)");
        const auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            QRegularExpression extract(R"((?<=id\=)[0-9]+)");
            auto match = extract.match(value);
            if (!match.hasMatch()) {
                *ok = false;
                return {};
            }
            value = match.captured(0);
        }
    } else if (key == "spotifyId") {
        QRegularExpression primary(R"(^[0-9a-zA-Z]+$)");
        auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            QRegularExpression extract(R"((?<=track/)[0-9a-zA-Z]+)");
            auto match = extract.match(value);
            if (!match.hasMatch()) {
                *ok = false;
                return {};
            }
            value = match.captured(0);
        }
    } else if (key == "appleMusicId") {
        QRegularExpression primary(R"(^[0-9]+$)");
        auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            QRegularExpression extract(R"((?<=i\=)[0-9]+)");
            auto match = extract.match(value);
            if (!match.hasMatch()) {
                *ok = false;
                return {};
            }
            value = match.captured(0);
        }
    } else if (key == "qqMusicId") {
        QRegularExpression primary(R"(^[0-9a-zA-Z]+$)");
        auto legal = primary.match(value);
        if (!legal.hasMatch()) {
            QNetworkAccessManager manager;
            auto request = QNetworkRequest(QUrl(value));
            QNetworkReply* reply = manager.get(request);

            // 阻塞等待请求完成
            QEventLoop loop;
            QTimer timer;
            timer.setSingleShot(true);

            // 连接信号（完成/超时）
            connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            connect(&timer, &QTimer::timeout, [&](){
                loop.quit();
                reply->abort();  // 主动终止请求
            });

            timer.start(10*1000);
            loop.exec();

            // 判断超时情况
            bool isTimeout = !timer.isActive();
            if(isTimeout) {
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
                *ok = false;
                return {};
            }

            // 计算起始位置（目标字符串末尾后n个字符）
            int startPos = targetIndex + 18;
            if (startPos >= content.length()) {
                *ok = false;
                return {};
            }

            // 正则匹配
            QRegularExpression re("[0-9A-Za-z]+");
            QRegularExpressionMatch match = re.match(content, startPos);

            if (!match.hasMatch()) {
                *ok = false;
                return {};
            }
            value = match.captured(0);
        }
    }
    return value;
}

void IDModel::refreshAll() {
    beginResetModel(); // 通知视图数据即将全部重置
    endResetModel();   // 通知视图数据重置完成
}
