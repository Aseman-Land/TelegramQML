#define DEFINE_DIS \
    QPointer<TelegramPeerDetails> dis = this;

#include "telegrampeerdetails.h"
#include "telegramsharedpointer.h"
#include "telegramtools.h"
#include "telegramengine.h"
#include "telegramshareddatamanager.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QJSValue>
#include <QPointer>
#include <QQmlEngine>
#include <QtQml>

class TelegramPeerDetailsPrivate
{
public:
    TelegramSharedPointer<TelegramTypeQObject> peerRoot;
    TelegramSharedPointer<DialogObject> dialog;
    TelegramSharedPointer<UserObject> user;
    TelegramSharedPointer<ChatObject> chat;
    TelegramSharedPointer<UserFullObject> userFull;
    TelegramSharedPointer<ChatFullObject> chatFull;

    QList< TelegramSharedPointer<UserObject> > chatUsers;

    QPointer<PeerObject> peer;
    QPointer<TelegramEngine> engine;
    QJSValue dateConvertorMethod;
};

TelegramPeerDetails::TelegramPeerDetails(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramPeerDetailsPrivate;
}

void TelegramPeerDetails::setPeer(PeerObject *peer)
{
    if(p->peer == peer)
        return;

    p->peer = peer;
    refresh();

    Q_EMIT peerChanged();
    Q_EMIT isChatChanged();
    Q_EMIT isUserChanged();
    Q_EMIT isChannelChanged();
}

PeerObject *TelegramPeerDetails::peer() const
{
    return p->peer;
}

bool TelegramPeerDetails::isChat() const
{
    if(!p->peer) return false;
    return (p->peer->classType() == PeerObject::TypePeerChat);
}

bool TelegramPeerDetails::isUser() const
{
    if(!p->peer) return false;
    return (p->peer->classType() == PeerObject::TypePeerUser);
}

bool TelegramPeerDetails::isChannel() const
{
    if(!p->peer) return false;
    return (p->peer->classType() == PeerObject::TypePeerChannel);
}

void TelegramPeerDetails::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramPeerDetails::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramPeerDetails::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramPeerDetails::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramPeerDetails::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramPeerDetails::engine() const
{
    return p->engine;
}

QJSValue TelegramPeerDetails::dateConvertorMethod() const
{
    return p->dateConvertorMethod;
}

void TelegramPeerDetails::setDateConvertorMethod(const QJSValue &method)
{
    if(p->dateConvertorMethod.isNull() && method.isNull())
        return;

    p->dateConvertorMethod = method;
    Q_EMIT dateConvertorMethodChanged();
}

QString TelegramPeerDetails::title() const
{
    if(p->chat)
        return p->chat->title();
    else
    if(p->user)
        return (p->user->firstName() + " " + p->user->lastName()).trimmed();
    else
        return "";
}

int TelegramPeerDetails::participantsCount() const
{
    if(!p->chat)
        return 0;

    switch(p->chat->classType())
    {
    case ChatObject::TypeChat:
    case ChatObject::TypeChatForbidden:
        return p->chat->participantsCount();
        break;

    case ChatObject::TypeChannel:
    case ChatObject::TypeChannelForbidden:
        if(p->chatFull)
            return p->chatFull->participantsCount();
        break;
    }
    return 0;
}

QString TelegramPeerDetails::statusText() const
{
    if(p->user)
    {
        switch(p->user->status()->classType())
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
            return tr("Last seen %1").arg(convetDate(QDateTime::fromTime_t(p->user->status()->wasOnline())));
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
    if(p->chat)
    {
        if(isChat())
            return tr("%1 members").arg(p->chat->participantsCount());
        else
        if(isChannel() && p->chatFull)
            return tr("%1 members").arg(p->chatFull->participantsCount());
    }

    return QString();
}

UserFullObject *TelegramPeerDetails::userFull() const
{
    return p->userFull;
}

ChatFullObject *TelegramPeerDetails::chatFull() const
{
    return p->chatFull;
}

QVariantList TelegramPeerDetails::chatUsers() const
{
    QVariantList result;
    Q_FOREACH(const TelegramSharedPointer<UserObject> &user, p->chatUsers)
        result << QVariant::fromValue<QObject*>(user.data());
    return result;
}

void TelegramPeerDetails::refresh()
{
    connectDialogSignals(p->dialog, true);
    connectUserSignals(p->user, true);
    connectChatSignals(p->chat, true);

    if(!p->engine || !p->peer || !p->engine->telegram() || !p->engine->sharedData())
    {
        p->dialog = 0;
        p->user = 0;
        p->chat = 0;
        p->userFull = 0;
        p->chatFull = 0;
        p->chatUsers.clear();
        Q_EMIT titleChanged();
        Q_EMIT userFullChanged();
        Q_EMIT chatFullChanged();
        Q_EMIT chatUsersChanged();
        Q_EMIT participantsCountChanged();
        Q_EMIT statusTextChanged();
        return;
    }

    QPointer<TelegramSharedDataManager> tsdm = p->engine->sharedData();

    const QByteArray &key = TelegramTools::identifier(p->peer->core());

    p->peerRoot = TelegramTools::objectRoot(p->peer);
    p->dialog = tsdm->getDialog(key);
    p->user = tsdm->getUser(key);
    p->chat = tsdm->getChat(key);
    p->userFull = tsdm->getUserFull(key);
    p->chatFull = tsdm->getChatFull(key);
    p->chatUsers.clear();

    connectDialogSignals(p->dialog);
    connectUserSignals(p->user);
    connectChatSignals(p->chat);

    Telegram *tg = p->engine->telegram();
    DEFINE_DIS;
    if(p->user)
    {
        InputUser user(InputUser::typeInputUser);
        user.setUserId(p->user->id());
        user.setAccessHash(p->user->accessHash());
        tg->usersGetFullUser(user, [this, dis, tsdm](TG_USERS_GET_FULL_USER_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            p->userFull = tsdm->insertUserFull(result);
            Q_EMIT userFullChanged();
        });
    }
    if(p->chat)
    {
        switch(p->chat->classType())
        {
        case ChatObject::TypeChat:
        case ChatObject::TypeChatForbidden:
            tg->messagesGetFullChat(p->chat->id(), [this, dis](TG_MESSAGES_GET_FULL_CHAT_CALLBACK){
                Q_UNUSED(msgId)
                if(!dis) return;
                if(!error.null) {
                    setError(error.errorText, error.errorCode);
                    return;
                }
                insertChatFull(result);
            });
            break;

        case ChatObject::TypeChannel:
        case ChatObject::TypeChannelForbidden:
        {
            InputChannel channel(InputChannel::typeInputChannel);
            channel.setChannelId(p->chat->id());
            channel.setAccessHash(p->chat->accessHash());
            tg->channelsGetFullChannel(channel, [this, dis](TG_CHANNELS_GET_FULL_CHANNEL_CALLBACK){
                Q_UNUSED(msgId)
                if(!dis) return;
                if(!error.null) {
                    setError(error.errorText, error.errorCode);
                    return;
                }
                insertChatFull(result);
            });
        }
            break;
        }
    }

    Q_EMIT titleChanged();
    Q_EMIT userFullChanged();
    Q_EMIT chatFullChanged();
    Q_EMIT chatUsersChanged();
    Q_EMIT participantsCountChanged();
    Q_EMIT statusTextChanged();
}

void TelegramPeerDetails::connectChatSignals(ChatObject *chat, bool dc)
{
    if(!chat)
        return;

    if(dc) {
        disconnect(chat, &ChatObject::titleChanged, this, &TelegramPeerDetails::titleChanged);
        disconnect(chat, &ChatObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
    } else {
        connect(chat, &ChatObject::titleChanged, this, &TelegramPeerDetails::titleChanged);
        connect(chat, &ChatObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
    }
}

void TelegramPeerDetails::connectUserSignals(UserObject *user, bool dc)
{
    if(!user)
        return;

    if(dc) {
        disconnect(user, &UserObject::firstNameChanged, this, &TelegramPeerDetails::titleChanged);
        disconnect(user, &UserObject::lastNameChanged, this, &TelegramPeerDetails::titleChanged);
    } else {
        connect(user, &UserObject::firstNameChanged, this, &TelegramPeerDetails::titleChanged);
        connect(user, &UserObject::lastNameChanged, this, &TelegramPeerDetails::titleChanged);
    }
}

void TelegramPeerDetails::connectDialogSignals(DialogObject *dialog, bool dc)
{
    if(!dialog)
        return;
}

void TelegramPeerDetails::insertChatFull(const MessagesChatFull &result)
{
    if(!p->engine || !p->engine->sharedData())
        return;
    if(p->chatFull)
    {
        disconnect(p->chatFull.data(), &ChatFullObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
    }

    TelegramSharedDataManager *tsdm = p->engine->sharedData();

    p->chatUsers.clear();
    Q_FOREACH(const User &user, result.users())
        p->chatUsers << tsdm->insertUser(user);

    p->chatFull = tsdm->insertChatFull(result.fullChat());
    if(p->chatFull)
    {
        connect(p->chatFull.data(), &ChatFullObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
    }

    Q_EMIT chatFullChanged();
    Q_EMIT chatUsersChanged();
    Q_EMIT participantsCountChanged();
    Q_EMIT statusTextChanged();
}

QString TelegramPeerDetails::convetDate(const QDateTime &td) const
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
            return days? "Tomorrow " + td.toString("HH:mm") : td.toString("HH:mm");
        else
            return td.toString("MMM dd, HH:mm");
    }
}

TelegramPeerDetails::~TelegramPeerDetails()
{
    delete p;
}

