/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "telegramwallpapersmodel.h"
#include "telegramqml.h"
#include "objects/types.h"

#include <telegram.h>

#include <QDebug>

class TelegramWallpapersModelPrivate
{
public:
    TelegramQml *telegram;
    bool initializing;

    QList<qint64> wallpapers;
};

TelegramWallpapersModel::TelegramWallpapersModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new TelegramWallpapersModelPrivate;
    p->telegram = 0;
    p->initializing = false;
}

TelegramQml *TelegramWallpapersModel::telegram() const
{
    return p->telegram;
}

void TelegramWallpapersModel::setTelegram(TelegramQml *tgo)
{
    TelegramQml *tg = static_cast<TelegramQml*>(tgo);
    if( p->telegram == tg )
        return;

    p->telegram = tg;
    p->initializing = tg;
    emit telegramChanged();
    emit initializingChanged();
    if( !p->telegram )
        return;

    connect( p->telegram, SIGNAL(wallpapersChanged()), SLOT(wallpapersChanged()) );

    Telegram *tgObject = p->telegram->telegram();
    tgObject->accountGetWallPapers();
}

qint64 TelegramWallpapersModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return p->wallpapers.at(row);
}

int TelegramWallpapersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->wallpapers.count();
}

QVariant TelegramWallpapersModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    const qint64 key = id(index);
    switch( role )
    {
    case ItemRole:
        res = QVariant::fromValue<WallPaperObject*>(p->telegram->wallpaper(key));
        break;
    }

    return res;
}

QHash<qint32, QByteArray> TelegramWallpapersModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( ItemRole, "item");
    return *res;
}

int TelegramWallpapersModel::count() const
{
    return p->wallpapers.count();
}

bool TelegramWallpapersModel::initializing() const
{
    return p->initializing;
}

void TelegramWallpapersModel::wallpapersChanged()
{
    p->initializing = false;
    emit initializingChanged();

    const QList<qint64> & walls = p->telegram->wallpapers();

    beginResetModel();
    p->wallpapers.clear();
    endResetModel();

    for( int i=0 ; i<walls.count() ; i++ )
    {
        const qint64 dId = walls.at(i);
        if( p->wallpapers.contains(dId) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->wallpapers.insert( i, dId );
        endInsertRows();
    }
}

TelegramWallpapersModel::~TelegramWallpapersModel()
{
    delete p;
}
