#define DEFINE_DIS \
    QPointer<TelegramMessageListModel> dis = this;

#define PROCESS_ROW_CHANGE(KEY, ROLES) \
    int row = p->list.indexOf(KEY); \
    if(row >= 0) \
        Q_EMIT dataChanged(index(row), index(row), QVector<int>()ROLES);

#include "telegrammessagelistmodel.h"
#include "telegramtools.h"
#include "telegramshareddatamanager.h"
#include "private/telegrammessageiohandleritem.h"

#include <QDateTime>
#include <QQmlEngine>
#include <QPointer>
#include <QtQml>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>
#include <telegram.h>

class TelegramMessageListItem
{
public:
    TelegramMessageListItem() {}
    virtual ~TelegramMessageListItem() {}
    QByteArray id;
    TelegramSharedPointer<ChatObject> chat;
    TelegramSharedPointer<UserObject> user;
    TelegramSharedPointer<MessageObject> message;

    TelegramSharedPointer<UserObject> fwdUser;
    TelegramSharedPointer<ChatObject> fwdChat;
    TelegramSharedPointer<UserObject> replyUser;
    TelegramSharedPointer<ChatObject> replyChat;
    TelegramSharedPointer<MessageObject> replyMsg;

    QPointer<TelegramMessageIOHandlerItem> ioHandler;
};

class TelegramMessageListModelPrivate
{
public:
    qint64 lastRequest;
    bool refreshing;
    QList<QByteArray> list;
    QHash<QByteArray, TelegramMessageListItem> items;
    QSet<QObject*> connecteds;

    TelegramSharedPointer<InputPeerObject> currentPeer;
    QSet<QByteArray> sendings;
    QJSValue dateConvertorMethod;
};

TelegramMessageListModel::TelegramMessageListModel(QObject *parent) :
    TelegramAbstractEngineListModel(parent)
{
    p = new TelegramMessageListModelPrivate;
    p->refreshing = false;
    p->lastRequest = 0;
}

bool TelegramMessageListModel::refreshing() const
{
    return p->refreshing;
}

QByteArray TelegramMessageListModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramMessageListModel::count() const
{
    return p->list.count();
}

QVariant TelegramMessageListModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const QByteArray &key = TelegramMessageListModel::id(index);
    const TelegramMessageListItem &item = p->items.value(key);

    switch(role)
    {
    case RoleMessageItem:
        result = QVariant::fromValue<MessageObject*>(item.message);
        break;
    case RoleMediaItem:
        result = QVariant::fromValue<MessageMediaObject*>(item.message->media());
        break;
    case RoleServiceItem:
        result = QVariant::fromValue<MessageActionObject*>(item.message->action());
        break;
    case RoleMarkupItem:
        result = QVariant::fromValue<ReplyMarkupObject*>(item.message->replyMarkup());
        break;
    case RoleEntityList:
        break;
    case RoleFromUserItem:
        result = QVariant::fromValue<UserObject*>(item.user);
        break;
    case RoleToPeerItem:
        result = QVariant::fromValue<InputPeerObject*>(p->currentPeer);
        break;

    case RoleMessage:
        result = item.message->message();
        break;
    case RoleDate:
        result = convertDate( QDateTime::fromTime_t(item.message->date()) );
        break;
    case RoleUnread:
        result = item.message->unread();
        break;
    case RoleSent:
        result = !p->sendings.contains(item.id);
        break;
    case RoleOut:
        result = item.message->out();
        break;
    case RoleIsSticker:
    {
        result = false;
        if(item.message)
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeSticker)
                {
                    result = true;
                    break;
                }
    }
        break;
    case RoleIsAnimated:
    {
        result = false;
        if(item.message)
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAnimated)
                {
                    result = true;
                    break;
                }
    }
        break;
    case RoleReplyMessage:
        result = QVariant::fromValue<MessageObject*>(item.replyMsg);
        break;
    case RoleReplyMsgId:
        if(item.message)
            result = item.message->replyToMsgId();
        else
            result = 0;
        break;
    case RoleReplyPeer:
        if(item.replyUser) result = QVariant::fromValue<UserObject*>(item.replyUser);
        else
        if(item.replyChat) result = QVariant::fromValue<ChatObject*>(item.replyChat);
        break;
    case RoleForwardFromPeer:
        if(item.fwdUser) result = QVariant::fromValue<UserObject*>(item.fwdUser);
        else
        if(item.fwdChat) result = QVariant::fromValue<ChatObject*>(item.fwdChat);
        break;
    case RoleForwardDate:
        result = QDateTime::fromTime_t(item.message->fwdFrom()->date());
        break;

    case RoleDownloadable:
    {
        MessageMediaObject *media = item.message->media();
        if(media)
            result = (media->classType() == MessageMediaObject::TypeMessageMediaDocument ||
                      media->classType() == MessageMediaObject::TypeMessageMediaPhoto);
        else
            result = false;
    }
        break;
    case RoleDownloading:
        result = (item.ioHandler && item.ioHandler->status() == TelegramMessageIOHandlerItem::StatusDownloading);
        break;
    case RoleDownloaded:
        result = (item.ioHandler && item.ioHandler->status() == TelegramMessageIOHandlerItem::StatusDone);
        break;
    case RoleTransfaredSize:
        if(item.ioHandler)
            result = item.ioHandler->transfaredSize();
        else
            result = 0;
        break;

    case RoleTotalSize:
        if(item.ioHandler)
            result = item.ioHandler->totalSize();
        else
            result = 0;
        break;

    case RoleFilePath:
        if(item.ioHandler)
            result = item.ioHandler->file();
        break;

    case RoleThumbPath:
        break;
    }
    return result;
}

QHash<int, QByteArray> TelegramMessageListModel::roleNames() const
{
    static QHash<int, QByteArray> *result = 0;
    if(result)
        return *result;

    result = new QHash<int, QByteArray>();
    result->insert(RoleMessage, "message");
    result->insert(RoleDate, "date");
    result->insert(RoleUnread, "unread");
    result->insert(RoleSent, "sent");
    result->insert(RoleOut, "out");
    result->insert(RoleIsSticker, "isSticker");
    result->insert(RoleIsAnimated, "isAnimated");
    result->insert(RoleReplyMsgId, "replyMsgId");
    result->insert(RoleReplyMessage, "replyMessage");
    result->insert(RoleReplyPeer, "replyPeer");
    result->insert(RoleForwardFromPeer, "forwardFromPeer");
    result->insert(RoleForwardDate, "forwardDate");

    result->insert(RoleMessageItem, "item");
    result->insert(RoleMediaItem, "chat");
    result->insert(RoleServiceItem, "user");
    result->insert(RoleMarkupItem, "topMessage");
    result->insert(RoleEntityList, "topMessage");
    result->insert(RoleFromUserItem, "fromUserItem");
    result->insert(RoleToPeerItem, "toPeerItem");

    result->insert(RoleDownloadable, "downloadable");
    result->insert(RoleDownloading, "downloading");
    result->insert(RoleDownloaded, "downloaded");
    result->insert(RoleTransfaredSize, "transfaredSize");
    result->insert(RoleTotalSize, "totalSize");
    result->insert(RoleFilePath, "filePath");
    result->insert(RoleThumbPath, "thumbPath");

    return *result;
}

void TelegramMessageListModel::setCurrentPeer(InputPeerObject *peer)
{
    if(p->currentPeer == peer)
        return;

    p->currentPeer = peer;
    refresh();
    Q_EMIT currentPeerChanged();
}

InputPeerObject *TelegramMessageListModel::currentPeer() const
{
    return p->currentPeer;
}

QJSValue TelegramMessageListModel::dateConvertorMethod() const
{
    return p->dateConvertorMethod;
}

void TelegramMessageListModel::setDateConvertorMethod(const QJSValue &method)
{
    if(p->dateConvertorMethod.isNull() && method.isNull())
        return;

    p->dateConvertorMethod = method;
    Q_EMIT dateConvertorMethodChanged();
}

bool TelegramMessageListModel::sendMessage(const QString &message, MessageObject *replyTo, ReplyMarkupObject *replyMarkup)
{
    TelegramMessageIOHandlerItem *handler = new TelegramMessageIOHandlerItem(this);
    handler->setEngine(mEngine);
    handler->setCurrentPeer(p->currentPeer);
    handler->setText(message);
    handler->setReplyTo(replyTo);
    handler->setReplyMarkup(replyMarkup);

    connect(handler, &TelegramMessageIOHandlerItem::statusChanged, this, [this, handler](){
        if(mEngine != handler->engine() || p->currentPeer != handler->currentPeer())
            return;
        if(!handler->result() || handler->status() != TelegramMessageIOHandlerItem::StatusDone)
            return;

        TelegramSharedDataManager *tsdm = mEngine->sharedData();
        QByteArray key;
        TelegramMessageListItem item;
        item.message = tsdm->insertMessage(handler->result()->core(), &key);
        item.user = tsdm->insertUser(mEngine->our()->user()->core());
        item.id = key;
        if(handler->replyTo())
            item.replyMsg = tsdm->insertMessage(handler->replyTo()->core());

        p->items[item.id] = item;
        delete handler;
        resort();
    }, Qt::QueuedConnection);

    if(!handler->send())
    {
        delete handler;
        return false;
    }

    resort();
    return true;
}

bool TelegramMessageListModel::sendFile(int type, const QString &file, MessageObject *replyTo, ReplyMarkupObject *replyMarkup)
{
    TelegramMessageIOHandlerItem *handler = new TelegramMessageIOHandlerItem(this);
    handler->setEngine(mEngine);
    handler->setCurrentPeer(p->currentPeer);
    handler->setFile(file);
    handler->setSendFileType(type);
    handler->setReplyTo(replyTo);
    handler->setReplyMarkup(replyMarkup);

    connect(handler, &TelegramMessageIOHandlerItem::statusChanged, this, [this, handler](){
        if(mEngine != handler->engine() || p->currentPeer != handler->currentPeer())
            return;
        if(!handler->result() || handler->status() != TelegramMessageIOHandlerItem::StatusDone)
            return;

        TelegramSharedDataManager *tsdm = mEngine->sharedData();
        QByteArray key;
        TelegramMessageListItem item;
        item.message = tsdm->insertMessage(handler->result()->core(), &key);
        item.user = tsdm->insertUser(mEngine->our()->user()->core());
        item.id = key;
        if(handler->replyTo())
            item.replyMsg = tsdm->insertMessage(handler->replyTo()->core());

        p->items[item.id] = item;
        delete handler;
        resort();
    }, Qt::QueuedConnection);

    if(!handler->send())
    {
        delete handler;
        return false;
    }

    resort();

    return true;
}

void TelegramMessageListModel::refresh()
{
    clean();
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;

    getMessagesFromServer(0, 100);
}

void TelegramMessageListModel::clean()
{
    changed(QHash<QByteArray, TelegramMessageListItem>());
}

void TelegramMessageListModel::resort()
{
    Q_FOREACH(const QByteArray &key, p->sendings)
        p->items.remove(key);

    p->sendings.clear();
    changed(p->items);
}

void TelegramMessageListModel::setRefreshing(bool stt)
{
    if(p->refreshing == stt)
        return;

    p->refreshing = stt;
    Q_EMIT refreshingChanged();
}

QByteArray TelegramMessageListModel::identifier() const
{
    return p->currentPeer? TelegramTools::identifier(p->currentPeer->core()) : QByteArray();
}

QString TelegramMessageListModel::convertDate(const QDateTime &td) const
{
    QQmlEngine *engine = qmlEngine(this);
    if(p->dateConvertorMethod.isCallable() && engine)
        return p->dateConvertorMethod.call(QJSValueList()<<engine->toScriptValue<QDateTime>(td)).toString();
    else
    if(!p->dateConvertorMethod.isNull() && !p->dateConvertorMethod.isUndefined())
        return p->dateConvertorMethod.toString();
    else
    {
        const QDateTime &current = QDateTime::currentDateTime();
        const qint64 secs = td.secsTo(current);
        const int days = td.daysTo(current);
        if(secs < 24*60*60)
            return days? "Yesterday " + td.toString("HH:mm") : td.toString("HH:mm");
        else
            return td.toString("MMM dd, HH:mm");
    }
}

void TelegramMessageListModel::getMessagesFromServer(int offset, int limit, QHash<QByteArray, TelegramMessageListItem> *items)
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;
    if(!items)
        items = new QHash<QByteArray, TelegramMessageListItem>();

    setRefreshing(true);

    const InputPeer &input = p->currentPeer->core();
    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    p->lastRequest = tg->messagesGetHistory(input, 0, 0, offset, limit, 0, 0, [=](TG_MESSAGES_GET_HISTORY_CALLBACK){
        if(!dis || p->lastRequest != msgId) {
            delete items;
            return;
        }

        setRefreshing(false);

        if(!error.null) {
            setError(error.errorText, error.errorCode);
            clean();
            delete items;
            return;
        }

        processOnResult(result, items);
        changed(*items);
        delete items;
    });
}

void TelegramMessageListModel::processOnResult(const MessagesMessages &result, QHash<QByteArray, TelegramMessageListItem> *items)
{
    TelegramSharedDataManager *tsdm = mEngine->sharedData();

    QHash<qint32, QByteArray> messagePeers;
    QHash<qint32, QByteArray> messageForwardsUsers;
    QHash<qint32, QByteArray> messageForwardsChats;

    Q_FOREACH(const Message &msg, result.messages())
    {
        QByteArray key;
        TelegramMessageListItem item;
        item.message = tsdm->insertMessage(msg, &key);
        item.id = key;
        (*items)[key] = item;

        messagePeers[msg.fromId()] = key;
        if(msg.fwdFrom().fromId())
            messageForwardsUsers.insertMulti(msg.fwdFrom().fromId(), key);
        if(msg.fwdFrom().channelId())
            messageForwardsChats.insertMulti(msg.fwdFrom().channelId(), key);

        connectMessageSignals(key, item.message);
    }

    Q_FOREACH(const Chat &chat, result.chats())
    {
        if(messagePeers.contains(chat.id()))
        {
            const QByteArray &key = messagePeers.value(chat.id());
            TelegramMessageListItem &item = (*items)[key];
            item.chat = tsdm->insertChat(chat);
            connectChatSignals(key, item.chat);
        }
        if(messageForwardsChats.contains(chat.id()))
        {
            QList<QByteArray> keys = messageForwardsChats.values(chat.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.fwdChat = tsdm->insertChat(chat);
                connectChatSignals(key, item.chat);
            }
        }
    }

    Q_FOREACH(const User &user, result.users())
    {
        if(messagePeers.contains(user.id()))
        {
            const QByteArray &key = messagePeers.value(user.id());
            TelegramMessageListItem &item = (*items)[key];
            item.user = tsdm->insertUser(user);
            connectUserSignals(key, item.user);
        }
        if(messageForwardsUsers.contains(user.id()))
        {
            QList<QByteArray> keys = messageForwardsUsers.values(user.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.fwdUser = tsdm->insertUser(user);
                connectUserSignals(key, item.user);
            }
        }
    }
}

void TelegramMessageListModel::changed(QHash<QByteArray, TelegramMessageListItem> items)
{
    TelegramSharedDataManager *tsdm = mEngine->sharedData();
    QList<QByteArray> list = getSortedList(items);
    if(mEngine && mEngine->our() && p->currentPeer)
    {
        QList<TelegramMessageIOHandlerItem*> handlerItems = TelegramMessageIOHandlerItem::getItems(mEngine, p->currentPeer);
        Q_FOREACH(TelegramMessageIOHandlerItem *handler, handlerItems)
        {
            MessageObject *handlerMsg = handler->result();
            if(!handlerMsg)
                continue;

            for(int i=0; i<list.count(); i++)
            {
                const QByteArray &l = list.at(i);
                MessageObject *msg = items.value(l).message;
                if(!msg)
                    continue;
                if(msg->date() > handlerMsg->date())
                    continue;

                QByteArray key;
                TelegramMessageListItem item;
                item.message = tsdm->insertMessage(handlerMsg->core(), &key);
                item.user = tsdm->insertUser(mEngine->our()->user()->core());
                if(handler->replyTo())
                    item.replyMsg = tsdm->insertMessage(handler->replyTo()->core());
                item.id = key;
                item.ioHandler = handler;
                items[key] = item;
                list.removeAll(key);
                list.insert(i, key);
                p->sendings.insert(item.id);
                connectHandlerSignals(item.id, item.ioHandler);
                break;
            }
        }
    }

    p->items.unite(items);

    bool count_changed = (list.count()==p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const QByteArray &item = p->list.at(i);
        if( list.contains(item) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<QByteArray> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const QByteArray &item = temp_list.at(i);
        if( p->list.contains(item) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const QByteArray &item = p->list.at(i);
            int nw = temp_list.indexOf(item);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const QByteArray &item = list.at(i);
        if( p->list.contains(item) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, item );
        endInsertRows();
    }

    p->items = items;
    if(count_changed)
        Q_EMIT countChanged();
}

const QHash<QByteArray, TelegramMessageListItem> *tg_mlist_model_lessthan_items = 0;
bool tg_mlist_model_sort(const QByteArray &s1, const QByteArray &s2);

QByteArrayList TelegramMessageListModel::getSortedList(const QHash<QByteArray, TelegramMessageListItem> &items)
{
    tg_mlist_model_lessthan_items = &items;
    QByteArrayList list = items.keys();
    qStableSort(list.begin(), list.end(), tg_mlist_model_sort);
    return list;
}

void TelegramMessageListModel::connectMessageSignals(const QByteArray &id, MessageObject *message)
{
    if(!message || p->connecteds.contains(message)) return;
    connect(message, &MessageObject::unreadChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleUnread);
    });
    p->connecteds.insert(message);
    connect(message, &MessageObject::destroyed, this, [this, message](){ if(p) p->connecteds.remove(message); });
}

void TelegramMessageListModel::connectChatSignals(const QByteArray &id, ChatObject *chat)
{
    if(!chat || p->connecteds.contains(chat)) return;

    p->connecteds.insert(chat);
    connect(chat, &ChatObject::destroyed, this, [this, chat](){ if(p) p->connecteds.remove(chat); });
}

void TelegramMessageListModel::connectUserSignals(const QByteArray &id, UserObject *user)
{
    if(!user || p->connecteds.contains(user)) return;

    p->connecteds.insert(user);
    connect(user, &UserObject::destroyed, this, [this, user](){ if(p) p->connecteds.remove(user); });
}

void TelegramMessageListModel::connectHandlerSignals(const QByteArray &id, TelegramMessageIOHandlerItem *handler)
{
    if(!handler || p->connecteds.contains(handler)) return;

    connect(handler, &TelegramMessageIOHandlerItem::transfaredSizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTransfaredSize);
    });
    connect(handler, &TelegramMessageIOHandlerItem::totalSizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTotalSize);
    });
    connect(handler, &TelegramMessageIOHandlerItem::statusChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleDownloaded << RoleDownloading);
    });

    p->connecteds.insert(handler);
    connect(handler, &TelegramMessageIOHandlerItem::destroyed, this, [this, handler](){ if(p) p->connecteds.remove(handler); });
}

TelegramMessageListModel::~TelegramMessageListModel()
{
    TelegramMessageListModelPrivate *tmp = p;
    p = 0;
    delete tmp;
}


bool tg_mlist_model_sort(const QByteArray &s1, const QByteArray &s2)
{
    const TelegramMessageListItem &i1 = tg_mlist_model_lessthan_items->value(s1);
    const TelegramMessageListItem &i2 = tg_mlist_model_lessthan_items->value(s2);
    return i1.message->id() > i2.message->id();
}
