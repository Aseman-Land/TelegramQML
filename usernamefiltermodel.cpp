#include "usernamefiltermodel.h"
#include "telegramqml.h"
#include "objects/types.h"

#include <QPointer>

class UserNameFilterModelPrivate
{
public:
    QPointer<TelegramQml> telegram;
    QPointer<DialogObject> dialog;

    QList<qint64> list;
    QString keyword;
};

UserNameFilterModel::UserNameFilterModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new UserNameFilterModelPrivate;
}

TelegramQml *UserNameFilterModel::telegram() const
{
    return p->telegram;
}

void UserNameFilterModel::setTelegram(TelegramQml *tgo)
{
    TelegramQml *tg = static_cast<TelegramQml*>(tgo);
    if( p->telegram == tg )
        return;

    if(p->telegram)
        disconnect(p->telegram, SIGNAL(chatFullsChanged()), this, SLOT(refresh()));

    p->telegram = tg;
    if(p->telegram)
        connect(p->telegram, SIGNAL(chatFullsChanged()), this, SLOT(refresh()));

    emit telegramChanged();
    listChanged();
}

DialogObject *UserNameFilterModel::dialog() const
{
    return p->dialog;
}

void UserNameFilterModel::setDialog(DialogObject *dialog)
{
    if(p->dialog == dialog)
        return;

    p->dialog = dialog;
    emit dialogChanged();

    listChanged();
}

qint64 UserNameFilterModel::id(const QModelIndex &index) const
{
    const int row = index.row();
    return p->list.at(row);
}

int UserNameFilterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant UserNameFilterModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const qint64 userId = id(index);
    switch(role)
    {
    case UserIdRole:
        result = userId;
        break;
    }

    return result;
}

QHash<qint32, QByteArray> UserNameFilterModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( UserIdRole, "userId");
    return *res;
}

int UserNameFilterModel::count() const
{
    return p->list.count();
}

void UserNameFilterModel::setKeyword(const QString &keyword)
{
    if(p->keyword == keyword)
        return;

    p->keyword = keyword;
    emit keywordChanged();

    listChanged();
}

QString UserNameFilterModel::keyword() const
{
    return p->keyword;
}

void UserNameFilterModel::refresh()
{
    listChanged();
}

qint64 UserNameFilterModel::get(int idx)
{
    if(idx < 0 || idx >= p->list.count())
        return 0;

    return p->list.at(idx);
}

void UserNameFilterModel::listChanged()
{
    QList<qint64> list;
    if(p->telegram)
        list = p->telegram->userIndex(p->keyword);

    QList<qint64> dialogList;
    if(p->telegram && p->dialog)
    {
        qint64 chatId = p->dialog->peer()->chatId();
        if(chatId)
        {
            ChatFullObject *chatFull = p->telegram->chatFull(chatId);
            if(chatFull)
                dialogList = chatFull->participants()->participants()->userIds();
            else
                p->telegram->messagesGetFullChat(chatId);
        }
        else
            dialogList << p->dialog->peer()->userId();
    }

    if(!dialogList.isEmpty())
        for( int i=0 ; i<list.count() ; i++ )
        {
            const qint64 uId = list.at(i);
            if( dialogList.contains(uId) )
                continue;

            list.removeAt(i);
            i--;
        }

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const qint64 uId = p->list.at(i);
        if( list.contains(uId) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }


    QList<qint64> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const qint64 uId = temp_list.at(i);
        if( p->list.contains(uId) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const qint64 uId = p->list.at(i);
            int nw = temp_list.indexOf(uId);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }


    for( int i=0 ; i<list.count() ; i++ )
    {
        const qint64 uId = list.at(i);
        if( p->list.contains(uId) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, uId );
        endInsertRows();
    }

    emit countChanged();
}

UserNameFilterModel::~UserNameFilterModel()
{
    delete p;
}

