#include "telegramnotificationhandler.h"
#include "telegramengine.h"
#include "telegram.h"
#include "telegramsharedpointer.h"
#include "telegramshareddatamanager.h"

#include <QPointer>

class TelegramNotificationHandlerPrivate
{
public:
    QPointer<TelegramEngine> engine;
    QPointer<Telegram> telegram;
};

TelegramNotificationHandler::TelegramNotificationHandler(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramNotificationHandlerPrivate;
}

void TelegramNotificationHandler::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramNotificationHandler::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramNotificationHandler::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramNotificationHandler::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramNotificationHandler::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramNotificationHandler::engine() const
{
    return p->engine;
}

void TelegramNotificationHandler::refresh()
{
    if(!p->engine || !p->engine->telegram() || !p->engine->sharedData())
        return;

    if(p->telegram)
    {
        disconnect(p->telegram.data(), &Telegram::updates, this, &TelegramNotificationHandler::onUpdates);
        disconnect(p->telegram.data(), &Telegram::updatesCombined, this, &TelegramNotificationHandler::onUpdatesCombined);
        disconnect(p->telegram.data(), &Telegram::updateShort, this, &TelegramNotificationHandler::onUpdateShort);
        disconnect(p->telegram.data(), &Telegram::updateShortChatMessage, this, &TelegramNotificationHandler::onUpdateShortChatMessage);
        disconnect(p->telegram.data(), &Telegram::updateShortMessage, this, &TelegramNotificationHandler::onUpdateShortMessage);
    }

    p->telegram = p->engine->telegram();
    if(p->telegram)
    {
        connect(p->telegram.data(), &Telegram::updates, this, &TelegramNotificationHandler::onUpdates);
        connect(p->telegram.data(), &Telegram::updatesCombined, this, &TelegramNotificationHandler::onUpdatesCombined);
        connect(p->telegram.data(), &Telegram::updateShort, this, &TelegramNotificationHandler::onUpdateShort);
        connect(p->telegram.data(), &Telegram::updateShortChatMessage, this, &TelegramNotificationHandler::onUpdateShortChatMessage);
        connect(p->telegram.data(), &Telegram::updateShortMessage, this, &TelegramNotificationHandler::onUpdateShortMessage);
    }
}

void TelegramNotificationHandler::onUpdateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out)
{
    if(!p->engine)
        return;

    Peer peer(Peer::typePeerUser);
    peer.setUserId(out? userId : p->engine->telegram()->ourId());

    Message msg(Message::typeMessage);
    msg.setId(id);
    msg.setFromId(out? p->engine->telegram()->ourId() : userId);
    msg.setToId(peer);
    msg.setMessage(message);
    msg.setDate(date);
    msg.setFwdFrom(fwd_from);
    msg.setReplyToMsgId(reply_to_msg_id);
    msg.setUnread(unread);
    msg.setOut(out);

    Update update(Update::typeUpdateNewMessage);
    update.setMessage(msg);
    update.setPts(pts);
    update.setPtsCount(pts_count);

    insertUpdate(update);
}

void TelegramNotificationHandler::onUpdateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out)
{
    Peer peer(Peer::typePeerChat);
    peer.setChatId(chatId);

    Message msg(Message::typeMessage);
    msg.setId(id);
    msg.setFromId(fromId);
    msg.setToId(peer);
    msg.setMessage(message);
    msg.setDate(date);
    msg.setFwdFrom(fwd_from);
    msg.setReplyToMsgId(reply_to_msg_id);
    msg.setUnread(unread);
    msg.setOut(out);

    Update update(Update::typeUpdateNewMessage);
    update.setMessage(msg);
    update.setPts(pts);
    update.setPtsCount(pts_count);

    insertUpdate(update);
}

void TelegramNotificationHandler::onUpdateShort(const Update &update, qint32 date)
{
    Q_UNUSED(date)
    insertUpdate(update);
}

void TelegramNotificationHandler::onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq)
{
    if(!p->engine || !p->engine->sharedData())
        return;

    TelegramSharedDataManager *tsdm = p->engine->sharedData();

    QSet< TelegramSharedPointer<TelegramTypeQObject> > cache;
    Q_FOREACH(const User &user, users)
        cache.insert( tsdm->insertUser(user).data() );
    Q_FOREACH(const Chat &chat, chats)
        cache.insert( tsdm->insertChat(chat).data() );
    Q_FOREACH(const Update &update, updates)
        insertUpdate(update);

    // Cache will clear at the end of the function
}

void TelegramNotificationHandler::onUpdates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq)
{
    if(!p->engine || !p->engine->sharedData())
        return;

    TelegramSharedDataManager *tsdm = p->engine->sharedData();

    QSet< TelegramSharedPointer<TelegramTypeQObject> > cache;
    Q_FOREACH(const User &user, users)
        cache.insert( tsdm->insertUser(user).data() );
    Q_FOREACH(const Chat &chat, chats)
        cache.insert( tsdm->insertChat(chat).data() );
    Q_FOREACH(const Update &update, udts)
        insertUpdate(update);

    // Cache will clear at the end of the function
}

void TelegramNotificationHandler::insertUpdate(const Update &update)
{
    if(!p->engine)
        return;

    Telegram *tg = p->engine->telegram();
    TelegramSharedDataManager *tsdm = p->engine->sharedData();
    if(!tg || !tsdm)
        return;

    const uint type = static_cast<int>(update.classType());
    switch(type)
    {
    case Update::typeUpdateNewChannelMessage:
    case Update::typeUpdateNewMessage:
    {
        const Message &msg = update.message();
        Peer msgPeer = TelegramTools::messagePeer(msg);
        QByteArray msgPeerKey = TelegramTools::identifier(msgPeer);

        QString title;
        if(update.classType()==Update::typeUpdateNewChannelMessage) {
            Peer fromPeer;
            fromPeer.setChannelId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerChannel);

            QByteArray fromPeerKey = TelegramTools::identifier(fromPeer);
            TelegramSharedPointer<ChatObject> chat = tsdm->getChat(fromPeerKey);
            if(chat)
                title = chat->title();
        } else {
            Peer fromPeer;
            fromPeer.setUserId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerUser);

            QByteArray fromPeerKey = TelegramTools::identifier(fromPeer);
            TelegramSharedPointer<UserObject> user = tsdm->getUser(fromPeerKey);
            if(user)
                title = (user->firstName() + " " + user->lastName()).trimmed();
        }

        Q_EMIT newMessage(title, msg.message(), msgPeerKey.toHex());
    }
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteMessages:
        break;
    case Update::typeUpdateUserTyping:
    case Update::typeUpdateChatUserTyping:
        break;
    case Update::typeUpdateChatParticipants:
        break;
    case Update::typeUpdateUserStatus:
        break;
    case Update::typeUpdateUserName:
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

TelegramNotificationHandler::~TelegramNotificationHandler()
{
    delete p;
}

