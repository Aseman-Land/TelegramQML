#include "stickersmodel.h"
#include "telegramqml.h"
#include "objects/types.h"

#include <telegram.h>

#include <QPointer>
#include <QDebug>

class StickersModelPrivate
{
public:
    QPointer<TelegramQml> telegram;

    bool initializing;
    QList<qint64> list;
};

StickersModel::StickersModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new StickersModelPrivate;
    p->initializing = false;
}

TelegramQml *StickersModel::telegram() const
{
    return p->telegram;
}

void StickersModel::setTelegram(TelegramQml *tgo)
{
    if(p->telegram == tgo)
        return;
    if(p->telegram)
        disconnect(p->telegram, SIGNAL(stickersChanged()), this, SLOT(listChanged()));

    p->telegram = tgo;
    if(p->telegram)
        connect(p->telegram, SIGNAL(stickersChanged()), this, SLOT(listChanged()));

    Q_EMIT telegramChanged();
}

qint64 StickersModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int StickersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant StickersModel::data(const QModelIndex &index, int role) const
{
    const qint64 sid = id(index);
    QVariant result;
    switch(role)
    {
    case stickerIdRole:
        result = sid;
        break;

    case documentRole:
        if(p->telegram)
            result = QVariant::fromValue<DocumentObject*>(p->telegram->sticker(sid));
        break;

    case emoticonRole:
        break;

    case stickerSetNameRole:
        if(p->telegram)
            result = stickerOfDocument(p->telegram->sticker(sid)).shortName();
        break;

    case stickerSetIdRole:
        if(p->telegram)
            result = stickerOfDocument(p->telegram->sticker(sid)).id();
        break;

    case stickerSetAccessHashRole:
        if(p->telegram)
            result = stickerOfDocument(p->telegram->sticker(sid)).accessHash();
        break;

    }

    return result;
}

QHash<qint32, QByteArray> StickersModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( stickerIdRole, "stickerId");
    res->insert( documentRole, "document");
    res->insert( emoticonRole, "emoticon");
    res->insert( stickerSetNameRole, "stickerSetName");
    res->insert( stickerSetIdRole, "stickerSetId");
    res->insert( stickerSetAccessHashRole, "stickerSetAccessHash");
    return *res;
}

int StickersModel::count() const
{
    return p->list.count();
}

bool StickersModel::initializing() const
{
    return p->initializing;
}

void StickersModel::refresh()
{
    if(!p->telegram || !p->telegram->telegram())
        return;
    if(!p->telegram->authLoggedIn())
        return;

    p->telegram->telegram()->messagesGetAllStickers(QString());
}

void StickersModel::listChanged()
{
    if(!p->telegram)
        return;

    const QList<qint64> &list = p->telegram->stickers();

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const qint64 dId = p->list.at(i);
        if( list.contains(dId) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }


    QList<qint64> temp_msgs = list;
    for( int i=0 ; i<temp_msgs.count() ; i++ )
    {
        const qint64 dId = temp_msgs.at(i);
        if( p->list.contains(dId) )
            continue;

        temp_msgs.removeAt(i);
        i--;
    }
    while( p->list != temp_msgs )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const qint64 dId = p->list.at(i);
            int nw = temp_msgs.indexOf(dId);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }


    for( int i=0 ; i<list.count() ; i++ )
    {
        const qint64 dId = list.at(i);
        if( p->list.contains(dId) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, dId );
        endInsertRows();
    }

    Q_EMIT countChanged();
}

InputStickerSet StickersModel::stickerOfDocument(DocumentObject *obj) const
{
    const QList<DocumentAttribute> &attrs = obj->attributes();
    Q_FOREACH(const DocumentAttribute &attr, attrs)
        if(attr.classType() == DocumentAttribute::typeDocumentAttributeSticker)
            return attr.stickerset();

    return InputStickerSet();
}

StickersModel::~StickersModel()
{
    delete p;
}

