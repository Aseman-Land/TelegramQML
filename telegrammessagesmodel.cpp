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

#define LOAD_STEP_COUNT 50

#include "telegrammessagesmodel.h"
#include "telegramqml.h"
#include "database.h"
#include "newsletterdialog.h"
#include "objects/types.h"

#include <telegram.h>
#include <QPointer>


class TelegramMessagesModelPrivate
{
public:
    QPointer<TelegramQml> telegram;
    bool initializing;
    bool refreshing;
    bool refreshing_cache;
    int maxId;
    int stepCount;

    QList<qint64> messages;
    QPointer<DialogObject> dialog;

    int load_count;
    int load_limit;
    int refresh_timer;

    int unreadCount;
};

TelegramMessagesModel::TelegramMessagesModel(QObject *parent) :
    TgAbstractListModel(parent)
{
    p = new TelegramMessagesModelPrivate;
    p->telegram = 0;
    p->initializing = false;
    p->refreshing = false;
    p->refreshing_cache = false;
    p->load_count = 0;
    p->load_limit = 0;
    p->refresh_timer = 0;
    p->maxId = 0;
    p->stepCount = LOAD_STEP_COUNT;
    p->unreadCount = 0;
}

TelegramQml *TelegramMessagesModel::telegram() const
{
    return p->telegram;
}

void TelegramMessagesModel::setTelegram(TelegramQml *tgo)
{
    TelegramQml *tg = static_cast<TelegramQml*>(tgo);
    if( p->telegram == tg )
        return;
    if(p->telegram)
    {
        p->telegram->unregisterMessagesModel(this);
        disconnect(p->telegram, SIGNAL(messagesChanged(bool)), this, SLOT(messagesChanged(bool)));
        disconnect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(init()));
        disconnect(p->telegram, SIGNAL(connectedChanged()), this, SLOT(init()));
        disconnect(p->telegram, SIGNAL(connectedChanged()), this, SLOT(setReaded()));
    }

    p->telegram = tg;
    if( p->telegram )
    {
        p->telegram->registerMessagesModel(this);
        connect(p->telegram, SIGNAL(messagesChanged(bool)), this, SLOT(messagesChanged(bool)));
        connect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(init()), Qt::QueuedConnection);
        connect(p->telegram, SIGNAL(connectedChanged()), this, SLOT(init()), Qt::QueuedConnection);
        connect(p->telegram, SIGNAL(connectedChanged()), this, SLOT(setReaded()), Qt::QueuedConnection);
    }

    p->initializing = tg;
    Q_EMIT telegramChanged();
    Q_EMIT initializingChanged();

    init();
}

DialogObject *TelegramMessagesModel::dialog() const
{
    return p->dialog;
}

void TelegramMessagesModel::setDialog(DialogObject *dlg)
{
    if( p->dialog == dlg )
        return;

    p->dialog = dlg;
    Q_EMIT dialogChanged();

    beginResetModel();
    p->messages.clear();
    endResetModel();

    if( !p->dialog )
        return;
    if( !p->dialog->peer()->chatId() && !p->dialog->peer()->userId() )
        return;

    p->unreadCount = p->dialog->unreadCount();
    Q_EMIT hasNewMessageChanged();

    init();
}

void TelegramMessagesModel::setMaxId(int id)
{
    if(p->maxId == id)
        return;

    p->maxId = id;
    Q_EMIT maxIdChanged();

    init();
}

int TelegramMessagesModel::maxId() const
{
    return p->maxId;
}

void TelegramMessagesModel::setStepCount(int step)
{
    if(p->stepCount == step)
        return;

    p->stepCount = qMax(0, step); // Avoid < 0 values
    Q_EMIT stepCountChanged();
}

int TelegramMessagesModel::stepCount() const
{
    return p->stepCount;
}

int TelegramMessagesModel::indexOf(qint64 msgId) const
{
    return p->messages.indexOf(msgId);
}

void TelegramMessagesModel::init()
{
    if( !p->dialog )
        return;
    if( !p->telegram || !p->telegram->authLoggedIn() )
        return;
    if( p->dialog == p->telegram->nullDialog() )
        return;

    p->load_count = 0;
    p->load_limit = p->stepCount;
    loadMore(true);
    messagesChanged(true);

    if(p->dialog->peer()->userId() != NewsLetterDialog::cutegramId())
    {
        p->refreshing = true;
        Q_EMIT refreshingChanged();
    }
}

void TelegramMessagesModel::refresh()
{
    if( !p->telegram )
        return;
    if( !p->dialog )
        return;
    if(p->dialog == p->telegram->nullDialog())
        return;

    Telegram *tgObject = p->telegram->telegram();
    if(!tgObject)
        return;

    if(p->dialog->encrypted())
    {
        Peer peer(Peer::typePeerChat);
        peer.setChatId(p->dialog->peer()->userId());

        p->telegram->database()->readMessages(peer, 0, p->stepCount);
        return;
    }

    const InputPeer & peer = p->telegram->getInputPeer(peerId());

    if(p->dialog->peer()->userId() != NewsLetterDialog::cutegramId())
        tgObject->messagesGetHistory(peer, 0, p->maxId, p->stepCount );

    p->telegram->database()->readMessages(TelegramMessagesModel::peer(), 0, p->stepCount);
}

void TelegramMessagesModel::loadMore(bool force)
{
    if( !p->telegram )
        return;
    if( !p->dialog )
        return;
    if( !force && p->messages.count() == 0 )
        return;
    if( !force && p->load_limit == p->load_count + p->stepCount)
        return;
    if(p->dialog == p->telegram->nullDialog())
        return;

    p->load_limit = p->load_count + p->stepCount;

    Telegram *tgObject = p->telegram->telegram();
    if(!tgObject)
        return;

    if(p->dialog->encrypted())
    {
        Peer peer(Peer::typePeerChat);
        peer.setChatId(p->dialog->peer()->userId());

        p->telegram->database()->readMessages(peer, p->load_count, p->stepCount);
        return;
    }

    const InputPeer & peer = p->telegram->getInputPeer(peerId());

    if(p->dialog->peer()->userId() != NewsLetterDialog::cutegramId())
    {
        if (p->telegram->connected())
        {
            tgObject->messagesGetHistory(peer, p->load_count, p->maxId, p->load_limit );
            p->refreshing = true;
        }
    }

    p->telegram->database()->readMessages(TelegramMessagesModel::peer(), p->load_count, p->stepCount);

    Q_EMIT refreshingChanged();
}

void TelegramMessagesModel::sendMessage(const QString &msg, int inReplyTo)
{
    if( !p->telegram )
        return;
    if( !p->dialog )
        return;

    clearNewMessageFlag();
    qint32 did = p->dialog->peer()->classType()==Peer::typePeerChat? p->dialog->peer()->chatId() : p->dialog->peer()->userId();
    p->telegram->sendMessage(did, msg, inReplyTo);
}

void TelegramMessagesModel::setReaded()
{
    if( !p->telegram )
        return;
    if( !p->dialog )
        return;
    if( p->telegram->invisible() )
        return;

    p->dialog->setUnreadCount(0);
    qint32 topMessageId = p->dialog->topMessage();
    if (topMessageId == 0) return;

    const MessageObject* message = p->telegram->message(topMessageId);
    if(!message) {
        qDebug() << __FUNCTION__ << ": Can't find message with id: " << topMessageId;
        return;
    }

    if(p->telegram->connected()) {
        p->telegram->messagesReadHistory(peerId(), message->date());
    }
}

void TelegramMessagesModel::clearNewMessageFlag()
{
    p->unreadCount = 0;
    Q_EMIT hasNewMessageChanged();
}

qint64 TelegramMessagesModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return p->messages.at(row);
}

int TelegramMessagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->messages.count();
}

QVariant TelegramMessagesModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    const qint64 key = id(index);
    switch( role )
    {
    case ItemRole:
        res = QVariant::fromValue<MessageObject*>(p->telegram->message(key));
        break;

    case UnreadedRole:
        res = index.row()<p->unreadCount;
        break;
    }

    return res;
}

QHash<qint32, QByteArray> TelegramMessagesModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( ItemRole, "item");
    res->insert( UnreadedRole, "unreaded");
    return *res;
}

int TelegramMessagesModel::count() const
{
    return p->messages.count();
}

bool TelegramMessagesModel::initializing() const
{
    return p->initializing;
}

bool TelegramMessagesModel::refreshing() const
{
    return p->refreshing;
}

bool TelegramMessagesModel::hasNewMessage() const
{
    return p->unreadCount;
}

qint64 TelegramMessagesModel::peerId() const
{
    bool isChat = p->dialog->peer()->classType()==Peer::typePeerChat;
    if(isChat)
        return p->dialog->peer()->chatId();
    else
        return p->dialog->peer()->userId();
}

Peer TelegramMessagesModel::peer() const
{
    Peer peer( static_cast<Peer::PeerType>(p->dialog->peer()->classType()) );
    peer.setChatId(p->dialog->peer()->chatId());
    peer.setUserId(p->dialog->peer()->userId());
    return peer;
}

void TelegramMessagesModel::messagesChanged(bool cachedData)
{
    if(!cachedData && p->refreshing)
    {
        p->refreshing = false;
        Q_EMIT refreshingChanged();
    }

    if(p->refresh_timer)
        killTimer(p->refresh_timer);

    p->refresh_timer = startTimer(100);
}

void TelegramMessagesModel::messagesChanged_priv()
{
    if( !p->dialog )
        return;

    qint32 did = p->dialog->peer()->classType()==Peer::typePeerChat? p->dialog->peer()->chatId() : p->dialog->peer()->userId();
    const QList<qint64> & messages = p->telegram->messages(did, p->maxId).mid(0,p->load_limit);

    for( int i=0 ; i<p->messages.count() ; i++ )
    {
        const qint64 msgId = p->messages.at(i);
        if( messages.contains(msgId) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->messages.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<qint64> temp_msgs = messages;
    for( int i=0 ; i<temp_msgs.count() ; i++ )
    {
        const qint64 msgId = temp_msgs.at(i);
        if( p->messages.contains(msgId) )
            continue;

        temp_msgs.removeAt(i);
        i--;
    }
    while( p->messages != temp_msgs )
        for( int i=0 ; i<p->messages.count() ; i++ )
        {
            const qint64 msgId = p->messages.at(i);
            int nw = temp_msgs.indexOf(msgId);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->messages.move( i, nw );
            endMoveRows();
        }


    for( int i=0 ; i<messages.count() ; i++ )
    {
        const qint64 msgId = messages.at(i);
        if( p->messages.contains(msgId) )
            continue;

        if(!p->refreshing_cache && !p->refreshing && i<p->unreadCount)
            p->unreadCount++;

        beginInsertRows(QModelIndex(), i, i );
        p->messages.insert( i, msgId );
        endInsertRows();

        Q_EMIT messageAdded(msgId);
    }

    p->load_count = p->messages.count();
    Q_EMIT countChanged();

    if(p->refreshing_cache && !p->refreshing)
        Q_EMIT focusToNewRequest(p->unreadCount);

    p->refreshing_cache = p->refreshing;
}

void TelegramMessagesModel::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == p->refresh_timer)
    {
        killTimer(p->refresh_timer);
        p->refresh_timer = 0;
        messagesChanged_priv();
    }

    TgAbstractListModel::timerEvent(e);
}

TelegramMessagesModel::~TelegramMessagesModel()
{
    if(p->telegram)
        p->telegram->unregisterMessagesModel(this);

    delete p;
}
