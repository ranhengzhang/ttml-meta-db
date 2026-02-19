//
// Created by LEGION on 25-3-7.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <QMap>

#include "types/artist.h"
#include "types/album.h"
#include "types/track.h"

class DataBase {
public:
    static QMap<QString,Artist> artists;
    static QMap<QString,Album> albums;
    static QMap<QString,Track> tracks;
};



#endif //DATABASE_H
