#define DEFINE_DIS \
    QPointer<TelegramMessageListModel> dis = this;

#define PROCESS_ROW_CHANGE(KEY, ROLES) \
    int row = p->list.indexOf(KEY); \
    if(row >= 0) \
        Q_EMIT dataChanged(index(row), index(row), QVector<int>()ROLES);

#include "telegrammessagelistmodel.h"
#include "telegramtools.h"
#include "telegramshareddatamanager.h"

#include <QDateTime>
#include <QQmlEngine>
#include <QPointer>
#include <QtQml>
#include <QDebug>
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
    TelegramSharedPointer<UserObject> replyUser;
    TelegramSharedPointer<MessageObject> replyMsg;
};

class TelegramMessageListModelPrivate
{
public:
    bool refreshing;
    QList<QByteArray> list;

    TelegramSharedPointer<DialogObject> currentDialog;
};

TelegramMessageListModel::TelegramMessageListModel(QObject *parent) :
    TelegramAbstractListModel(parent)
{
    p = new TelegramMessageListModelPrivate;
    p->refreshing = false;
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
    const QByteArray &id = TelegramMessageListModel::id(index);
    switch(role)
    {
    case RoleMessageItem:
        break;
    case RoleMediaItem:
        break;
    case RoleServiceItem:
        break;
    case RoleMarkupItem:
        break;
    case RoleEntityList:
        break;
    case RoleFromUserItem:
        break;
    case RoleToPeerItem:
        break;

    case RoleMessage:
        break;
    case RoleDate:
        break;
    case RoleUnread:
        break;
    case RoleSent:
        break;
    case RoleOut:
        break;
    case RoleReplyMessage:
        break;
    case RoleForwardFromPeer:
        break;
    case RoleForwardDate:
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
    result->insert(RoleReplyMessage, "message");
    result->insert(RoleForwardFromPeer, "forwardFromPeer");
    result->insert(RoleForwardDate, "forwardDate");

    result->insert(RoleMessageItem, "dialog");
    result->insert(RoleMediaItem, "chat");
    result->insert(RoleServiceItem, "user");
    result->insert(RoleMarkupItem, "topMessage");
    result->insert(RoleEntityList, "topMessage");
    result->insert(RoleFromUserItem, "fromUserItem");
    result->insert(RoleToPeerItem, "toPeerItem");
    return *result;
}

void TelegramMessageListModel::setCurrentDialog(DialogObject *dialog)
{
    if(p->currentDialog == dialog)
        return;

    p->currentDialog = dialog;
    refresh();
    Q_EMIT currentDialogChanged();
}

DialogObject *TelegramMessageListModel::currentDialog() const
{
    return p->currentDialog;
}

void TelegramMessageListModel::refresh()
{
    if(!mEngine || !mEngine->telegram() || !p->currentDialog)
    {
        clean();
        return;
    }

    getMessagesFromServer(0, 100);
}

void TelegramMessageListModel::clean()
{
    changed(QHash<QByteArray, TelegramMessageListItem>());
}

void TelegramMessageListModel::setRefreshing(bool stt)
{
    if(p->refreshing == stt)
        return;

    p->refreshing = stt;
    Q_EMIT refreshingChanged();
}

void TelegramMessageListModel::getMessagesFromServer(int offset, int limit, QHash<QByteArray, TelegramMessageListItem> *items)
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;
    if(!items)
        items = new QHash<QByteArray, TelegramMessageListItem>();

    setRefreshing(true);

    const InputPeer &input = TelegramTools::peerInputPeer(p->currentDialog->peer()->core());
    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    tg->messagesGetHistory(input, 0, 0, offset, limit, 0, 0, [=](TG_MESSAGES_GET_HISTORY_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) {
            delete items;
            return;
        }

        setRefreshing(false);

        if(!error.null) {
            setError(error.errorText, error.errorCode);
            delete items;
            return;
        }

        processOnResult(result, items);
        delete items;
    });
}

void TelegramMessageListModel::processOnResult(const MessagesMessages &result, QHash<QByteArray, TelegramMessageListItem> *items)
{
    TelegramSharedDataManager *tsdm = mEngine->sharedData();

    QHash<qint64, QByteArray> messagePeers;

    Q_FOREACH(const Message &msg, result.messages())
    {
        QByteArray key;
        TelegramMessageListItem item;
        item.message = tsdm->insertMessage(msg, &key);
        item.id = key;
        (*items)[key] = item;
        connectMessageSignals(key, item.message);
    }

    Q_FOREACH(const Chat &chat, result.chats())
    {
        if(!messagePeers.contains(chat.id()))
            continue;

        const QByteArray &key = messagePeers.value(chat.id());
        TelegramMessageListItem &item = (*items)[key];
        item.chat = tsdm->insertChat(chat);
        connectChatSignals(key, item.chat);
    }

    Q_FOREACH(const User &user, result.users())
    {
        if(!messagePeers.contains(user.id()))
            continue;

        const QByteArray &key = messagePeers.value(user.id());
        TelegramMessageListItem &item = (*items)[key];
        item.user = tsdm->insertUser(user);
        connectUserSignals(key, item.user);
    }
}

void TelegramMessageListModel::changed(const QHash<QByteArray, TelegramMessageListItem> &hash)
{

}

void TelegramMessageListModel::connectMessageSignals(const QByteArray &id, MessageObject *message)
{
    connect(message, &MessageObject::unreadChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleUnread);
    });
}

void TelegramMessageListModel::connectChatSignals(const QByteArray &id, ChatObject *chat)
{

}

void TelegramMessageListModel::connectUserSignals(const QByteArray &id, UserObject *user)
{

}

TelegramMessageListModel::~TelegramMessageListModel()
{
    delete p;
}

