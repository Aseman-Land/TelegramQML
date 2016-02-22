#define DEFINE_DIS \
    QPointer<TelegramDialogsListModel> dis = this;

#define PROCESS_ROW_CHANGE(ROLES) \
    int row = p->list.indexOf(id); \
    if(row >= 0) \
        Q_EMIT dataChanged(index(row), index(row), QVector<int>()ROLES);


#include "telegramdialogslistmodel.h"
#include "telegramtools.h"
#include "telegramshareddatamanager.h"

#include <QDateTime>
#include <QQmlEngine>
#include <QtQml>
#include <telegram.h>

class TelegramDialogsListItem
{
public:
    TelegramDialogsListItem() : dialog(0), chat(0), user(0), topMessage(0) {}
    virtual ~TelegramDialogsListItem() {}
    QByteArray id;
    TelegramSharedPointer<DialogObject> dialog;
    TelegramSharedPointer<ChatObject> chat;
    TelegramSharedPointer<UserObject> user;
    TelegramSharedPointer<MessageObject> topMessage;
};

class TelegramDialogsListModelPrivate
{
public:
    int visibility;
    QList<qint32> sortFlag;

    QHash<QByteArray, TelegramDialogsListItem> items;
    QList<QByteArray> list;

    int resortTimer;
    QJSValue dateConversation;

    QHash<int, int> typingUsers;
};

TelegramDialogsListModel::TelegramDialogsListModel(QObject *parent) :
    TelegramAbstractListModel(parent)
{
    p = new TelegramDialogsListModelPrivate;
    p->resortTimer = 0;
    p->visibility = VisibilityAll;
    p->sortFlag << SortByDate << SortByUnreads << SortByName << SortByType << SortByOnline;
}

int TelegramDialogsListModel::visibility() const
{
    return p->visibility;
}

void TelegramDialogsListModel::setVisibility(int visibility)
{
    if(p->visibility == visibility)
        return;

    p->visibility = visibility;
    Q_EMIT visibilityChanged();
}

QList<qint32> TelegramDialogsListModel::sortFlag() const
{
    return p->sortFlag;
}

void TelegramDialogsListModel::setSortFlag(const QList<qint32> &sortFlag)
{
    if(p->sortFlag == sortFlag)
        return;

    p->sortFlag = sortFlag;
    Q_EMIT sortFlagChanged();
}

QJSValue TelegramDialogsListModel::dateConversation() const
{
    return p->dateConversation;
}

void TelegramDialogsListModel::setDateConversation(const QJSValue &method)
{
    if(p->dateConversation.isNull() && method.isNull())
        return;

    p->dateConversation = method;
    Q_EMIT dateConversationChanged();
}

QByteArray TelegramDialogsListModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramDialogsListModel::count() const
{
    return p->list.count();
}

QVariant TelegramDialogsListModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const QByteArray &key = id(index);
    const TelegramDialogsListItem &item = p->items.value(key);
    switch(role)
    {
    case Qt::DisplayRole:
    case RoleName:
        if(item.user) result = QString("%1 %2").arg(item.user->firstName(), item.user->lastName()).trimmed();
        if(item.chat) result = item.chat->title();
        break;
    case RoleMessageDate:
        if(item.topMessage) result = convetDate(QDateTime::fromTime_t(item.topMessage->date()));
        break;
    case RoleMessage:
        if(item.topMessage) result = item.topMessage->message();
        else result = QString();
        break;
    case RoleMessageUnread:
        if(item.topMessage) result = item.topMessage->unread();
        else result = true;
        break;
    case RoleLastOnline:
        if(item.user) result = QDateTime::fromTime_t(item.user->status()->wasOnline());
        break;
    case RoleIsOnline:
        if(item.user) result = (item.user->status()->classType() == UserStatusObject::TypeUserStatusOnline);
        else result = false;
        break;
    case RoleStatus:
        if(item.user) result = item.user->status()->classType();
        else result = 0;
        break;
    case RoleStatusText:
        result = statusText(item);
        break;
    case RoleTyping:
        if(item.user) result = p->typingUsers.contains(item.user->id());
        else result = false;
        break;
    case RoleUnreadCount:
        result = item.dialog->unreadCount();
        break;
    case RoleDialogItem:
        result = QVariant::fromValue<DialogObject*>(item.dialog);
        break;
    case RoleChatItem:
        result = QVariant::fromValue<ChatObject*>(item.chat);
        break;
    case RoleUserItem:
        result = QVariant::fromValue<UserObject*>(item.user);
        break;
    case RoleTopMessageItem:
        result = QVariant::fromValue<MessageObject*>(item.topMessage);
        break;
    }
    return result;
}

QHash<int, QByteArray> TelegramDialogsListModel::roleNames() const
{
    static QHash<int, QByteArray> *result = 0;
    if(result)
        return *result;

    result = new QHash<int, QByteArray>();
    result->insert(RoleName, "title");
    result->insert(RoleMessageDate, "messageDate");
    result->insert(RoleMessageUnread, "messageUnread");
    result->insert(RoleMessage, "message");
    result->insert(RoleLastOnline, "lastOnline");
    result->insert(RoleIsOnline, "isOnline");
    result->insert(RoleStatus, "status");
    result->insert(RoleStatusText, "statusText");
    result->insert(RoleTyping, "typing");
    result->insert(RoleUnreadCount, "unreadCount");

    result->insert(RoleDialogItem, "dialog");
    result->insert(RoleChatItem, "chat");
    result->insert(RoleUserItem, "user");
    result->insert(RoleTopMessageItem, "topMessage");
    return *result;
}

void TelegramDialogsListModel::refresh()
{
    if(!mEngine || !mEngine->telegram())
    {
        clean();
        return;
    }

    getDialogsFromServer(InputPeer::null, 200);
}

void TelegramDialogsListModel::clean()
{
    changed(QHash<QByteArray, TelegramDialogsListItem>());
}

void TelegramDialogsListModel::resort()
{
    if(p->resortTimer)
        killTimer(p->resortTimer);
    p->resortTimer = QObject::startTimer(200);
}

void TelegramDialogsListModel::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == p->resortTimer)
    {
        changed(QHash<QByteArray, TelegramDialogsListItem>(p->items));
        killTimer(p->resortTimer);
        p->resortTimer = 0;
    }

    TelegramAbstractListModel::timerEvent(e);
}

void TelegramDialogsListModel::getDialogsFromServer(const InputPeer &offset, int limit, QHash<QByteArray, TelegramDialogsListItem> *items)
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;
    if(!items)
        items = new QHash<QByteArray, TelegramDialogsListItem>();

    int offsetId = offset.userId();
    if(!offsetId) offsetId = offset.chatId();
    if(!offsetId) offsetId = offset.channelId();

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    tg->messagesGetDialogs(0, offsetId, offset, limit, [this, items, limit, dis](TG_MESSAGES_GET_DIALOGS_CALLBACK) {
        Q_UNUSED(msgId)
        if(!dis) {
            delete items;
            return;
        }
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            delete items;
            return;
        }

        const InputPeer lastInputPeer = processOnResult(result, items);
        const int count = 0;//result.dialogs().count();
        if(count == 0 || count < limit) {
            /*! finished !*/
            changed(*items);
            delete items;
        } else {
            /*! There are also another dialogs !*/
            getDialogsFromServer(lastInputPeer, limit, items);
        }
    });
}

InputPeer TelegramDialogsListModel::processOnResult(const MessagesDialogs &result, QHash<QByteArray, TelegramDialogsListItem> *items)
{
    TelegramSharedDataManager *tsdm = mEngine->sharedData();

    Q_FOREACH(const Dialog &dlg, result.dialogs())
    {
        QByteArray key;
        TelegramDialogsListItem item;
        item.dialog = tsdm->insertDialog(dlg, &key);
        item.id = key;
        (*items)[key] = item;
        connectDialogSignals(key, item.dialog);
    }

    QHash<int, Chat> chats;
    Q_FOREACH(const Chat &chat, result.chats())
    {
        chats[chat.id()] = chat;
        const QByteArray &id = TelegramTools::identifier(chat);
        if(!items->contains(id))
            continue;

        TelegramSharedPointer<ChatObject> chatPtr = tsdm->insertChat(chat);
        (*items)[id].chat = chatPtr;
        connectChatSignals(id, chatPtr);
    }

    QHash<int, User> users;
    Q_FOREACH(const User &user, result.users())
    {
        users[user.id()] = user;
        const QByteArray &id = TelegramTools::identifier(user);
        if(!items->contains(id))
            continue;

        TelegramSharedPointer<UserObject> userPtr = tsdm->insertUser(user);
        (*items)[id].user = userPtr;
        connectUserSignals(id, userPtr);
    }

    Q_FOREACH(const Message &msg, result.messages())
    {
        const QByteArray &id = TelegramTools::identifier(msg.toId());
        if(!items->contains(id)) continue;
        if((*items)[id].dialog->topMessage() != msg.id()) continue;
        TelegramSharedPointer<MessageObject> msgPtr = tsdm->insertMessage(msg);
        (*items)[id].topMessage = msgPtr;
        connectMessageSignals(id, msgPtr);
    }

    Peer peer = result.dialogs().last().peer();
    InputPeer lastInputPeer;
    switch(static_cast<int>(peer.classType()))
    {
    case Peer::typePeerChannel:
        lastInputPeer = TelegramTools::chatInputPeer(chats.value(peer.channelId()));
        break;
    case Peer::typePeerChat:
        lastInputPeer = TelegramTools::chatInputPeer(chats.value(peer.chatId()));
        break;
    case Peer::typePeerUser:
        lastInputPeer = TelegramTools::userInputPeer(users.value(peer.userId()));
        break;
    }

    return lastInputPeer;
}

void TelegramDialogsListModel::changed(const QHash<QByteArray, TelegramDialogsListItem> &items)
{
    const QByteArrayList &list = getSortedList(items);
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

const QHash<QByteArray, TelegramDialogsListItem> *tg_dlist_model_lessthan_items = 0;
int tg_dlist_model_lessthan_sortFlag = 0;
bool tg_dlist_model_sort(const QByteArray &s1, const QByteArray &s2);

QByteArrayList TelegramDialogsListModel::getSortedList(const QHash<QByteArray, TelegramDialogsListItem> &items)
{
    QByteArrayList list = items.keys();

    QList<int> flagOrders = p->sortFlag;
    QList<int> defaultOrders;
    defaultOrders << SortByDate << SortByUnreads << SortByName << SortByType << SortByOnline;
    Q_FOREACH(int flag, defaultOrders)
        if(!flagOrders.contains(flag))
            flagOrders << flag;

    tg_dlist_model_lessthan_items = &items;
    for(int i=flagOrders.count()-1; i>=0; i--)
    {
        const int flag = flagOrders.at(i);
        tg_dlist_model_lessthan_sortFlag = flag;
        qStableSort(list.begin(), list.end(), tg_dlist_model_sort);
    }

    return list;
}

void TelegramDialogsListModel::connectChatSignals(const QByteArray &id, ChatObject *chat)
{
    connect(chat, &ChatObject::titleChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(<<RoleName<<Qt::DisplayRole)
    });
}

void TelegramDialogsListModel::connectUserSignals(const QByteArray &id, UserObject *user)
{
    std::function<void ()> callback = [this, id](){
        PROCESS_ROW_CHANGE(<<RoleName<<Qt::DisplayRole);
    };
    connect(user, &UserObject::firstNameChanged, this, callback);
    connect(user, &UserObject::lastNameChanged, this, callback);
    connect(user->status(), &UserStatusObject::coreChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(<<RoleStatusText<<RoleStatus<<RoleIsOnline);
        resort();
    });
}

void TelegramDialogsListModel::connectMessageSignals(const QByteArray &id, MessageObject *message)
{
    connect(message, &MessageObject::unreadChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(<<RoleMessageUnread);
    });
}

void TelegramDialogsListModel::connectDialogSignals(const QByteArray &id, DialogObject *dialog)
{
    connect(dialog, &DialogObject::topMessageChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(<<RoleMessage<<RoleMessageDate<<RoleMessageUnread);
    });
    connect(dialog, &DialogObject::unreadCountChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(<<RoleUnreadCount);
    });
}

void TelegramDialogsListModel::onUpdateShort(const Update &update, qint32 date)
{
    Q_UNUSED(date)
    insertUpdate(update);
}

void TelegramDialogsListModel::onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq)
{
    Q_FOREACH(const Update &update, updates)
        insertUpdate(update);
}

void TelegramDialogsListModel::onUpdates(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq)
{
    Q_FOREACH(const Update &update, updates)
        insertUpdate(update);
}

void TelegramDialogsListModel::insertUpdate(const Update &update)
{
    switch(static_cast<int>(update.classType()))
    {
    case Update::typeUpdateNewMessage:
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteMessages:
        break;
    case Update::typeUpdateUserTyping:
    {
        const qint32 userId = update.userId();
        p->typingUsers[userId]++;
        Q_FOREACH(TelegramDialogsListItem item, p->items)
            if(item.user && item.user->id() == userId)
            {
                const QByteArray &id = item.id;
                PROCESS_ROW_CHANGE(<<RoleTyping);
            }

        startTimer(3000, [this, userId](){
            int &count = p->typingUsers[userId];
            count--;
            if(count == 0) p->typingUsers.remove(userId);
            Q_FOREACH(TelegramDialogsListItem item, p->items)
                if(item.user && item.user->id() == userId)
                {
                    const QByteArray &id = item.id;
                    PROCESS_ROW_CHANGE(<<RoleTyping);
                }
        });
    }
        break;
    case Update::typeUpdateChatUserTyping:
        break;
    case Update::typeUpdateChatParticipants:
    {
        Q_FOREACH(TelegramDialogsListItem item, p->items)
            if(item.chat && item.chat->id() == update.participants().chatId())
                item.chat->setParticipantsCount(update.participants().participants().count());
    }
        break;
    case Update::typeUpdateUserStatus:
    {
        Q_FOREACH(TelegramDialogsListItem item, p->items)
            if(item.user && item.user->id() == update.userId())
                item.user->status()->operator =(update.status());
    }
        break;
    case Update::typeUpdateUserName:
    {
        Q_FOREACH(TelegramDialogsListItem item, p->items)
            if(item.user && item.user->id() == update.userId())
            {
                UserObject *user = item.user;
                user->setFirstName(update.firstName());
                user->setLastName(update.lastName());
                user->setUsername(update.username());
            }
    }
        break;
    case Update::typeUpdateUserPhoto:
        break;
    case Update::typeUpdateContactRegistered:
        break;
    case Update::typeUpdateContactLink:
        break;
    case Update::typeUpdateNewAuthorization:
        break;
    case Update::typeUpdateNewEncryptedMessage:
        break;
    case Update::typeUpdateEncryptedChatTyping:
        break;
    case Update::typeUpdateEncryption:
        break;
    case Update::typeUpdateEncryptedMessagesRead:
        break;
    case Update::typeUpdateChatParticipantAdd:
        break;
    case Update::typeUpdateChatParticipantDelete:
        break;
    case Update::typeUpdateDcOptions:
        break;
    case Update::typeUpdateUserBlocked:
        break;
    case Update::typeUpdateNotifySettings:
        break;
    case Update::typeUpdateServiceNotification:
        break;
    case Update::typeUpdatePrivacy:
        break;
    case Update::typeUpdateUserPhone:
        break;
    case Update::typeUpdateReadHistoryInbox:
        break;
    case Update::typeUpdateReadHistoryOutbox:
        break;
    case Update::typeUpdateWebPage:
        break;
    case Update::typeUpdateReadMessagesContents:
        break;
    case Update::typeUpdateChannelTooLong:
        break;
    case Update::typeUpdateChannel:
        break;
    case Update::typeUpdateChannelGroup:
        break;
    case Update::typeUpdateNewChannelMessage:
        break;
    case Update::typeUpdateReadChannelInbox:
        break;
    case Update::typeUpdateDeleteChannelMessages:
        break;
    case Update::typeUpdateChannelMessageViews:
        break;
    case Update::typeUpdateChatAdmins:
        break;
    case Update::typeUpdateChatParticipantAdmin:
        break;
    case Update::typeUpdateNewStickerSet:
        break;
    case Update::typeUpdateStickerSetsOrder:
        break;
    case Update::typeUpdateStickerSets:
        break;
    case Update::typeUpdateSavedGifs:
        break;
    case Update::typeUpdateBotInlineQuery:
        break;
    case Update::typeUpdateBotInlineSend:
        break;
    }
}

QString TelegramDialogsListModel::statusText(const TelegramDialogsListItem &item) const
{
    if(item.user)
    {
        switch(item.user->status()->classType())
        {
        case UserStatusObject::TypeUserStatusEmpty:
            return QString();
            break;
        case UserStatusObject::TypeUserStatusLastMonth:
            return tr("Last month");
            break;
        case UserStatusObject::TypeUserStatusLastWeek:
            return tr("Last week");
            break;
        case UserStatusObject::TypeUserStatusOffline:
            return tr("Last seen %1").arg(convetDate(QDateTime::fromTime_t(item.user->status()->wasOnline())));
            break;
        case UserStatusObject::TypeUserStatusOnline:
            return tr("Online");
            break;
        case UserStatusObject::TypeUserStatusRecently:
            return tr("Last seen recently");
            break;
        }
    }
    else
    if(item.chat)
        return tr("%1 members").arg(item.chat->participantsCount());

    return QString();
}

QString TelegramDialogsListModel::convetDate(const QDateTime &td) const
{
    QQmlEngine *engine = qmlEngine(this);
    if(p->dateConversation.isCallable() && engine)
        return p->dateConversation.call(QJSValueList()<<engine->toScriptValue<QDateTime>(td)).toString();
    else
    if(!p->dateConversation.isNull() && !p->dateConversation.isUndefined())
        return p->dateConversation.toString();
    else
    {
        const QDateTime &current = QDateTime::currentDateTime();
        const qint64 secs = td.secsTo(current);
        const int days = td.daysTo(current);
        if(secs < 24*60*60)
            return days? td.toString("Tomorrow HH:mm") : td.toString("HH:mm");
        else
            return td.toString("MMM dd, HH:mm");
    }
}

TelegramDialogsListModel::~TelegramDialogsListModel()
{
    delete p;
}



bool tg_dlist_model_sort(const QByteArray &s1, const QByteArray &s2)
{
    const TelegramDialogsListItem &i1 = tg_dlist_model_lessthan_items->value(s1);
    const TelegramDialogsListItem &i2 = tg_dlist_model_lessthan_items->value(s2);
    switch(tg_dlist_model_lessthan_sortFlag)
    {
    case TelegramDialogsListModel::SortByDate:
        if(!i1.topMessage) return false;
        if(!i2.topMessage) return true;
        return i1.topMessage->date() > i2.topMessage->date();
        break;
    case TelegramDialogsListModel::SortByName:
    {
        QString n1, n2;
        if(i1.dialog->peer()->classType() == PeerObject::TypePeerUser && i1.user)
            n1 = i1.user->firstName() + " " + i1.user->lastName();
        else
        if(i1.chat)
            n1 = i1.chat->title();
        if(i2.dialog->peer()->classType() == PeerObject::TypePeerUser && i2.user)
            n2 = i2.user->firstName() + " " + i2.user->lastName();
        else
        if(i2.chat)
            n2 = i2.chat->title();
        return n1.toLower() < n2.toLower();
    }
        break;
    case TelegramDialogsListModel::SortByOnline:
        if(!i2.user || i2.user->status()->classType() == UserStatusObject::TypeUserStatusEmpty) return true;
        if(!i1.user || i1.user->status()->classType() == UserStatusObject::TypeUserStatusEmpty) return false;
        if( i1.user->status()->classType() == UserStatusObject::TypeUserStatusOnline &&
            i2.user->status()->classType() == UserStatusObject::TypeUserStatusOffline)
            return true;
        else
        if( i1.user->status()->classType() == UserStatusObject::TypeUserStatusOffline &&
            i2.user->status()->classType() == UserStatusObject::TypeUserStatusOnline)
            return false;
        else
        if(i1.user->status()->classType() == i2.user->status()->classType())
            return i1.user->status()->wasOnline() > i2.user->status()->wasOnline();
        else
            return i1.user->status()->classType() < i2.user->status()->classType();
        break;
    case TelegramDialogsListModel::SortByType:
        if(i2.dialog->peer()->classType() == PeerObject::TypePeerUser)
            return false;
        else if(i1.dialog->peer()->classType() == PeerObject::TypePeerUser)
            return true;
        else if(i2.dialog->peer()->classType() == PeerObject::TypePeerChat)
            return false;
        else if(i1.dialog->peer()->classType() == PeerObject::TypePeerChat)
            return true;
        else
            return false;
        break;
    case TelegramDialogsListModel::SortByUnreads:
        return i1.dialog->unreadCount() >= i2.dialog->unreadCount();
        break;
    }
    return s1 < s2;
}
