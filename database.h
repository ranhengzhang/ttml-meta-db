//
// Created by LEGION on 25-3-7.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <QMap>

#include "artist.h"
#include "album.h"
#include "track.h"

class DataBase {
public:
    static QMap<QString,Artist> artists;
    static QMap<QString,Album> albums;
    static QMap<QString,Track> tracks;
};



#endif //DATABASE_H
