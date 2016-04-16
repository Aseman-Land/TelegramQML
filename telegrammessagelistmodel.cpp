#define DEFINE_DIS \
    QPointer<TelegramMessageListModel> dis = this;

#define PROCESS_ROW_CHANGE(KEY, ROLES) \
    int row = p->list.indexOf(KEY); \
    if(row >= 0) \
        Q_EMIT dataChanged(index(row), index(row), QVector<int>()ROLES);

#include "telegrammessagelistmodel.h"
#include "telegramtools.h"
#include "telegramshareddatamanager.h"
#include "private/telegramuploadhandler.h"

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

    QPointer<TelegramUploadHandler> ioHandler;
};

class TelegramMessageListModelPrivate
{
public:
    qint64 lastRequest;
    bool refreshing;
    bool hasBackMore;

    QList<QByteArray> list;
    QHash<QByteArray, TelegramMessageListItem> items;
    QSet<QObject*> connecteds;

    TelegramSharedPointer<InputPeerObject> currentPeer;
    QSet<QByteArray> sendings;
    QJSValue dateConvertorMethod;
    int limit;

    QHash<ChatObject*, QHash<UserObject*, int> > typingChats;
};

TelegramMessageListModel::TelegramMessageListModel(QObject *parent) :
    TelegramAbstractEngineListModel(parent)
{
    p = new TelegramMessageListModelPrivate;
    p->refreshing = false;
    p->hasBackMore = false;
    p->lastRequest = 0;
    p->limit = 100;
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
        if(item.message) {
            if(!item.message->media()->caption().isEmpty())
                result = item.message->media()->caption();
            else
                result = item.message->message();
        } else {
            result = "";
        }
        break;
    case RoleDateTime:
        result = convertDate( QDateTime::fromTime_t(item.message->date()) );
        break;
    case RoleDate:
        result = QDateTime::fromTime_t(item.message->date()).date().toString("yyyy-MM-dd");
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

    case RoleMessageType:
        result = static_cast<int>(messageType(item.message));
        break;

    case RoleFileName:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeFilename)
                {
                    result = attr.fileName();
                    break;
                }
        }
        break;
    case RoleFileMimeType:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document())
            result = item.message->media()->document()->mimeType();
        break;
    case RoleFileTitle:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio)
                {
                    result = attr.title();
                    break;
                }
        }
        break;
    case RoleFilePerformer:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio)
                {
                    result = attr.performer();
                    break;
                }
        }
        break;
    case RoleFileDuration:
        result = 0;
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio ||
                   attr.classType() == DocumentAttribute::typeDocumentAttributeVideo)
                {
                    result = attr.duration();
                    break;
                }
        }
        break;
    case RoleFileIsVoice:
        result = false;
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio)
                {
                    result = attr.voice();
                    break;
                }
        }
        break;
    case RoleFileSize:
        if(item.message && item.message->media() && item.message->media()->document())
            result = item.message->media()->document()->size();
        else
            result = 0;
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
    case RoleTransfaring:
        result = (item.ioHandler && item.ioHandler->status() == TelegramUploadHandler::StatusUploading);
        break;
    case RoleTransfared:
        result = (item.ioHandler && item.ioHandler->status() == TelegramUploadHandler::StatusDone);
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
    result->insert(RoleDateTime, "dateTime");
    result->insert(RoleDate, "date");
    result->insert(RoleUnread, "unread");
    result->insert(RoleSent, "sent");
    result->insert(RoleOut, "out");
    result->insert(RoleReplyMsgId, "replyMsgId");
    result->insert(RoleReplyMessage, "replyMessage");
    result->insert(RoleReplyPeer, "replyPeer");
    result->insert(RoleForwardFromPeer, "forwardFromPeer");
    result->insert(RoleForwardDate, "forwardDate");
    result->insert(RoleMessageType, "messageType");

    result->insert(RoleMessageItem, "item");
    result->insert(RoleMediaItem, "chat");
    result->insert(RoleServiceItem, "user");
    result->insert(RoleMarkupItem, "topMessage");
    result->insert(RoleEntityList, "topMessage");
    result->insert(RoleFromUserItem, "fromUserItem");
    result->insert(RoleToPeerItem, "toPeerItem");

    result->insert(RoleFileName, "fileName");
    result->insert(RoleFileMimeType, "fileMimeType");
    result->insert(RoleFileTitle, "fileTitle");
    result->insert(RoleFilePerformer, "filePerformer");
    result->insert(RoleFileDuration, "fileDuration");
    result->insert(RoleFileIsVoice, "fileIsVoice");
    result->insert(RoleFileSize, "fileSize");

    result->insert(RoleDownloadable, "downloadable");
    result->insert(RoleTransfaring, "transfaring");
    result->insert(RoleTransfared, "transfared");
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
    Q_EMIT keyChanged();
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

int TelegramMessageListModel::limit() const
{
    return p->limit;
}

void TelegramMessageListModel::setLimit(int limit)
{
    if(p->limit == limit)
        return;

    p->limit = limit;
    Q_EMIT limitChanged();
}

QByteArray TelegramMessageListModel::key() const
{
    if(p->currentPeer)
        return TelegramTools::identifier( TelegramTools::inputPeerPeer(p->currentPeer->core()) );
    else
        return QByteArray();
}

QVariantList TelegramMessageListModel::typingUsers() const
{
    QVariantList result;
    if(!mEngine || !mEngine->sharedData())
        return result;

    QPointer<TelegramSharedDataManager> tsdm = mEngine->sharedData();
    TelegramSharedPointer<ChatObject> chat = tsdm->getChat(key());

    QList<UserObject*> users = p->typingChats.value(chat).keys();
    Q_FOREACH(UserObject *user, users)
        result << QVariant::fromValue<QObject*>(user);
    return result;
}

bool TelegramMessageListModel::sendMessage(const QString &message, MessageObject *replyTo, ReplyMarkupObject *replyMarkup)
{
    TelegramUploadHandler *handler = new TelegramUploadHandler(this);
    handler->setEngine(mEngine);
    handler->setCurrentPeer(p->currentPeer);
    handler->setText(message);
    handler->setReplyTo(replyTo);
    handler->setReplyMarkup(replyMarkup);

    connect(handler, &TelegramUploadHandler::statusChanged, this, [this, handler](){
        if(mEngine != handler->engine() || p->currentPeer != handler->currentPeer())
            return;
        if(!handler->result() || handler->status() != TelegramUploadHandler::StatusDone)
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
        connectMessageSignals(item.id, item.message);
        resort();

        QByteArray toId = TelegramTools::identifier(item.message->toId()->core());
        TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(toId);
        if(dialog && dialog->topMessage() < item.message->id())
            dialog->setTopMessage(item.message->id());
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
    TelegramUploadHandler *handler = new TelegramUploadHandler(this);
    handler->setEngine(mEngine);
    handler->setCurrentPeer(p->currentPeer);
    handler->setFile(file);
    handler->setSendFileType(type);
    handler->setReplyTo(replyTo);
    handler->setReplyMarkup(replyMarkup);

    connect(handler, &TelegramUploadHandler::statusChanged, this, [this, handler](){
        if(mEngine != handler->engine() || p->currentPeer != handler->currentPeer())
            return;
        if(!handler->result() || handler->status() != TelegramUploadHandler::StatusDone)
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
        connectMessageSignals(item.id, item.message);
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

void TelegramMessageListModel::deleteMessage(const QList<qint32> &msgs)
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    Telegram::Callback<MessagesAffectedMessages> callback = [this, dis, msgs](TG_MESSAGES_DELETE_MESSAGES_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        QHash<QByteArray, TelegramMessageListItem> items = p->items;
        Q_FOREACH(TelegramMessageListItem item, items) {
            if(!item.message)
                continue;
            if(!msgs.contains(item.message->id()))
                continue;

            items.remove(item.id);
        }

        changed(items);
    };

    if(p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel) {
        InputChannel input(InputChannel::typeInputChannel);
        input.setChannelId(p->currentPeer->channelId());
        input.setAccessHash(p->currentPeer->accessHash());
        tg->channelsDeleteMessages(input, msgs, callback);
    } else {
        tg->messagesDeleteMessages(msgs, callback);
    }
}

void TelegramMessageListModel::forwardMessage(InputPeerObject *fromInputPeer, const QList<qint32> &msgs)
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
//    tg->messagesForwardMessages();
}

void TelegramMessageListModel::markAsRead()
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    QPointer<TelegramSharedDataManager> tsdm = mEngine->sharedData();

    const InputPeer &input = p->currentPeer->core();
    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    if(input.classType() == InputPeer::typeInputPeerChannel)
    {
        InputChannel inputChannel(InputChannel::typeInputChannel);
        inputChannel.setChannelId(input.channelId());
        inputChannel.setAccessHash(input.accessHash());
        tg->channelsReadHistory(inputChannel, 0, [this, dis, input, tsdm](TG_CHANNELS_READ_HISTORY_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }
            if(!result || !tsdm)
                return;

            const QByteArray &key = TelegramTools::identifier( TelegramTools::inputPeerPeer(input) );
            TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(key);
            if(dialog)
                dialog->setUnreadCount(0);
        });
    }
    else
    {
        tg->messagesReadHistory(input, 0, [this, dis, input, tsdm](TG_MESSAGES_READ_HISTORY_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }
            if(!tsdm)
                return;

            const QByteArray &key = TelegramTools::identifier( TelegramTools::inputPeerPeer(input) );
            TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(key);
            if(dialog)
                dialog->setUnreadCount(0);
        });
    }
}

void TelegramMessageListModel::loadFrom(qint32 msgId)
{
    if(!p->currentPeer || !mEngine)
        return;

    clean();
    getMessagesFromServer(msgId, -p->limit/2, p->limit);
}

void TelegramMessageListModel::loadBack()
{
    if(!p->hasBackMore || !p->currentPeer || !mEngine)
        return;

    int offsetId = 0;
    if(!p->list.isEmpty())
    {
        const QByteArray &lastKey = p->list.last();
        const TelegramMessageListItem &item = p->items.value(lastKey);
        if(item.message)
            offsetId = item.message->id();
    }

    getMessagesFromServer(offsetId, 0, p->limit);
}

void TelegramMessageListModel::loadFront()
{
    if(!p->hasBackMore || !p->currentPeer || !mEngine)
        return;

    int offsetId = 0;
    if(!p->list.isEmpty())
    {
        const QByteArray &lastKey = p->list.first();
        const TelegramMessageListItem &item = p->items.value(lastKey);
        if(item.message)
            offsetId = item.message->id();
    }

    getMessagesFromServer(offsetId, -p->limit, p->limit);
}

void TelegramMessageListModel::refresh()
{
    clean();
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;

    getMessagesFromServer(0, 0, p->limit);
}

void TelegramMessageListModel::clean()
{
    p->hasBackMore = true;
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

TelegramMessageListModel::MessageType TelegramMessageListModel::messageType(MessageObject *msg) const
{
    if(!msg)
        return TypeUnsupportedMessage;

    switch(static_cast<int>(msg->classType()))
    {
    case MessageObject::TypeMessage:
    {
        if(!msg->media())
            return TypeTextMessage;

        switch(static_cast<int>(msg->media()->classType()))
        {
        case MessageMediaObject::TypeMessageMediaEmpty:
            return TypeTextMessage;
            break;
        case MessageMediaObject::TypeMessageMediaPhoto:
            return TypePhotoMessage;
            break;
        case MessageMediaObject::TypeMessageMediaGeo:
            return TypeGeoMessage;
            break;
        case MessageMediaObject::TypeMessageMediaContact:
            return TypeContactMessage;
            break;
        case MessageMediaObject::TypeMessageMediaUnsupported:
            return TypeUnsupportedMessage;
            break;
        case MessageMediaObject::TypeMessageMediaDocument:
        {
            if(!msg->media()->document())
                return TypeTextMessage;

            Q_FOREACH(const DocumentAttribute &attr, msg->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAnimated)
                    return TypeAnimatedMessage;
            Q_FOREACH(const DocumentAttribute &attr, msg->media()->document()->attributes())
            {
                switch(static_cast<int>(attr.classType()))
                {
                case DocumentAttribute::typeDocumentAttributeAudio:
                    return TypeAudioMessage;
                    break;
                case DocumentAttribute::typeDocumentAttributeSticker:
                    return TypeStickerMessage;
                    break;
                case DocumentAttribute::typeDocumentAttributeVideo:
                    return TypeVideoMessage;
                    break;
                }
            }
            return TypeDocumentMessage;
        }
            break;
        case MessageMediaObject::TypeMessageMediaWebPage:
            return TypeWebPageMessage;
            break;
        case MessageMediaObject::TypeMessageMediaVenue:
            return TypeVenueMessage;
            break;
        }

        return TypeTextMessage;
    }
        break;

    case MessageObject::TypeMessageEmpty:
        return TypeUnsupportedMessage;
        break;

    case MessageObject::TypeMessageService:
        return TypeActionMessage;
        break;
    }

    return TypeUnsupportedMessage;
}

void TelegramMessageListModel::getMessagesFromServer(int offsetId, int addOffset, int limit)
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    setRefreshing(true);

    const InputPeer &input = p->currentPeer->core();
    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    p->lastRequest = tg->messagesGetHistory(input, offsetId, 0, addOffset, limit, 0, 0, [this, dis, limit](TG_MESSAGES_GET_HISTORY_CALLBACK){
        if(!dis || p->lastRequest != msgId) return;
        setRefreshing(false);
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        if(result.messages().count() < limit)
            p->hasBackMore = false;

        QHash<QByteArray, TelegramMessageListItem> items = p->items;
        processOnResult(result, &items);
        changed(items);
        fetchReplies(result.messages());
    });
}

void TelegramMessageListModel::fetchReplies(const QList<Message> &messages)
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    QHash<qint32, qint32> repliesMap;
    Q_FOREACH(const Message &msg, messages)
        if(msg.replyToMsgId())
            repliesMap[msg.replyToMsgId()] = msg.id();

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    tg->messagesGetMessages(repliesMap.keys(), [this, dis, repliesMap](TG_MESSAGES_GET_MESSAGES_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        TelegramSharedDataManager *tsdm = mEngine->sharedData();
        if(!tsdm) return;

        QHash<qint32, User> users;
        Q_FOREACH(const User &user, result.users()) users[user.id()] = user;
        QHash<qint32, Chat> chats;
        Q_FOREACH(const Chat &chat, result.chats()) chats[chat.id()] = chat;

        Q_FOREACH(const Message &msg, result.messages())
        {
            qint32 orgMsg = repliesMap.value(msg.id());
            if(!orgMsg) return;

            QByteArray key = TelegramTools::identifier(TelegramTools::inputPeerPeer(p->currentPeer->core()), orgMsg);
            if(!p->items.contains(key)) return;

            TelegramMessageListItem &item = p->items[key];
            item.replyMsg = tsdm->insertMessage(msg);
            if(users.contains(msg.fromId()))
                item.replyUser = tsdm->insertUser(users.value(msg.fromId()));
            else
            if(chats.contains(msg.fromId()))
                item.replyChat = tsdm->insertChat(chats.value(msg.fromId()));

            PROCESS_ROW_CHANGE(key, <<RoleReplyPeer<<RoleReplyMessage )
        }
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

        messagePeers.insertMulti(msg.fromId(), key);
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
            QList<QByteArray> keys = messagePeers.values(chat.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.chat = tsdm->insertChat(chat);
                connectChatSignals(key, item.chat);
            }
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
            QList<QByteArray> keys = messagePeers.values(user.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.user = tsdm->insertUser(user);
                connectUserSignals(key, item.user);
            }
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
        QList<TelegramUploadHandler*> handlerItems = TelegramUploadHandler::getItems(mEngine, p->currentPeer);
        Q_FOREACH(TelegramUploadHandler *handler, handlerItems)
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

    bool count_changed = (list.count()!=p->list.count());

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
    Q_UNUSED(id)
    if(!chat || p->connecteds.contains(chat)) return;

    p->connecteds.insert(chat);
    connect(chat, &ChatObject::destroyed, this, [this, chat](){ if(p) p->connecteds.remove(chat); });
}

void TelegramMessageListModel::connectUserSignals(const QByteArray &id, UserObject *user)
{
    Q_UNUSED(id)
    if(!user || p->connecteds.contains(user)) return;

    p->connecteds.insert(user);
    connect(user, &UserObject::destroyed, this, [this, user](){ if(p) p->connecteds.remove(user); });
}

void TelegramMessageListModel::connectHandlerSignals(const QByteArray &id, TelegramUploadHandler *handler)
{
    if(!handler || p->connecteds.contains(handler)) return;

    connect(handler, &TelegramUploadHandler::transfaredSizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTransfaredSize);
    });
    connect(handler, &TelegramUploadHandler::totalSizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTotalSize);
    });
    connect(handler, &TelegramUploadHandler::statusChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTransfared << RoleTransfaring);
    });

    p->connecteds.insert(handler);
    connect(handler, &TelegramUploadHandler::destroyed, this, [this, handler](){ if(p) p->connecteds.remove(handler); });
}

void TelegramMessageListModel::onUpdates(const UpdatesType &updates)
{
    if(!mEngine || !mEngine->sharedData())
        return;

    TelegramSharedDataManager *tsdm = mEngine->sharedData();
    QSet< TelegramSharedPointer<TelegramTypeQObject> > cache;

    switch(static_cast<int>(updates.classType()))
    {
    case UpdatesType::typeUpdatesTooLong:
        break;
    case UpdatesType::typeUpdateShortMessage:
    {
        Peer peer(Peer::typePeerUser);
        peer.setUserId(updates.out()? updates.userId() : mEngine->telegram()->ourId());

        Message msg(Message::typeMessage);
        msg.setId(updates.id());
        msg.setFromId(updates.out()? mEngine->telegram()->ourId() : updates.userId());
        msg.setToId(peer);
        msg.setMessage(updates.message());
        msg.setDate(updates.date());
        msg.setFwdFrom(updates.fwdFrom());
        msg.setReplyToMsgId(updates.replyToMsgId());
        msg.setUnread(updates.unread());
        msg.setOut(updates.out());
        msg.setEntities(updates.entities());
        msg.setViaBotId(updates.viaBotId());
        msg.setSilent(updates.silent());
        msg.setMentioned(updates.mentioned());
        msg.setMediaUnread(updates.mediaUnread());

        Update update(Update::typeUpdateNewMessage);
        update.setMessage(msg);
        update.setPts(updates.pts());
        update.setPtsCount(updates.ptsCount());

        insertUpdate(update);
    }
        break;
    case UpdatesType::typeUpdateShortChatMessage:
    {
        Peer peer(Peer::typePeerChat);
        peer.setChatId(updates.chatId());

        Message msg(Message::typeMessage);
        msg.setId(updates.id());
        msg.setFromId(updates.fromId());
        msg.setToId(peer);
        msg.setMessage(updates.message());
        msg.setDate(updates.date());
        msg.setFwdFrom(updates.fwdFrom());
        msg.setReplyToMsgId(updates.replyToMsgId());
        msg.setUnread(updates.unread());
        msg.setOut(updates.out());
        msg.setEntities(updates.entities());
        msg.setViaBotId(updates.viaBotId());
        msg.setSilent(updates.silent());
        msg.setMentioned(updates.mentioned());
        msg.setMediaUnread(updates.mediaUnread());

        Update update(Update::typeUpdateNewMessage);
        update.setMessage(msg);
        update.setPts(updates.pts());
        update.setPtsCount(updates.ptsCount());

        insertUpdate(update);
    }
        break;
    case UpdatesType::typeUpdateShort:
    {
        insertUpdate(updates.update());
    }
        break;
    case UpdatesType::typeUpdatesCombined:
    case UpdatesType::typeUpdates:
    {
        Q_FOREACH(const User &user, updates.users())
            cache.insert( tsdm->insertUser(user).data() );
        Q_FOREACH(const Chat &chat, updates.chats())
            cache.insert( tsdm->insertChat(chat).data() );
        Q_FOREACH(const Update &upd, updates.updates())
            insertUpdate(upd);
    }
        break;
    case UpdatesType::typeUpdateShortSentMessage:
        break;
    }

    // Cache will clear at the end of the function
}

void TelegramMessageListModel::insertUpdate(const Update &update)
{
    if(!mEngine || !p->currentPeer)
        return;

    Telegram *tg = mEngine->telegram();
    TelegramSharedDataManager *tsdm = mEngine->sharedData();
    if(!tg || !tsdm)
        return;

    const uint type = static_cast<int>(update.classType());
    switch(type)
    {
    case Update::typeUpdateNewChannelMessage:
    case Update::typeUpdateNewMessage:
    {
        const Message &msg = update.message();
        const Peer &peer = TelegramTools::messagePeer(msg);
        const QByteArray &id = TelegramTools::identifier(peer);
        if(id != key())
            return;

        Peer fromPeer;
        if(update.classType()==Update::typeUpdateNewChannelMessage) {
            fromPeer.setChannelId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerChannel);
        } else {
            fromPeer.setUserId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerUser);
        }

        QByteArray fromPeerKey = TelegramTools::identifier(fromPeer);

        TelegramMessageListItem item;
        item.user = tsdm->getUser(fromPeerKey);
        item.chat = tsdm->getChat(fromPeerKey);
        item.message = tsdm->insertMessage(msg, &item.id);

        QHash<QByteArray, TelegramMessageListItem> items = p->items;
        items[item.id] = item;

        connectMessageSignals(item.id, item.message);
        changed(items);
    }
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteChannelMessages:
    {
        if(p->currentPeer->channelId() != update.channelId())
            break;
    }
    case Update::typeUpdateDeleteMessages:
    {
        const QList<qint32> &messages = update.messages();

        QHash<QByteArray, TelegramMessageListItem> items = p->items;
        Q_FOREACH(TelegramMessageListItem item, items)
            if(item.message && messages.contains(item.message->id()))
                items.remove(item.id);

        changed(items);
    }
        break;
    case Update::typeUpdateUserTyping:
    case Update::typeUpdateChatUserTyping:
    {
        const qint32 userId = update.userId();
        const qint32 chatId = update.chatId();

        UserObject *user = 0;
        ChatObject *chat = 0;
        QByteArray id;
        Q_FOREACH(TelegramMessageListItem item, p->items)
        {
            if(item.user && item.user->id() == userId)
            {
                user = item.user;
                if(type == Update::typeUpdateUserTyping)
                    id = item.id;
            }
            else
            if(item.chat && item.chat->id() == chatId)
            {
                chat = item.chat;
                if(type == Update::typeUpdateChatUserTyping)
                    id = item.id;
            }
        }

        if(user)
        {
            p->typingChats[chat][user]++;
            Q_EMIT typingUsersChanged();
            startTimer(5000, [this, chat, user, id](){
                int &count = p->typingChats[chat][user];
                count--;
                if(count == 0) {
                    p->typingChats[chat].remove(user);
                    if(p->typingChats.value(chat).isEmpty())
                        p->typingChats.remove(chat);
                }
                Q_EMIT typingUsersChanged();
            });
        }
    }
        break;
    case Update::typeUpdateChatParticipants:
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.chat && item.chat->id() == update.participants().chatId())
                item.chat->setParticipantsCount(update.participants().participants().count());
    }
        break;
    case Update::typeUpdateUserStatus:
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.user && item.user->id() == update.userId())
                item.user->status()->operator =(update.status());
    }
        break;
    case Update::typeUpdateUserName:
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
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
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.user && item.user->id() == update.userId())
            {
                UserObject *user = item.user;
                user->photo()->operator =(update.photo());
            }
    }
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
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.user && item.user->id() == update.userId())
            {
                UserObject *user = item.user;
                user->setPhone(update.phone());
            }
    }
        break;
    case Update::typeUpdateReadHistoryInbox:
    {
        const QByteArray &id = TelegramTools::identifier(update.peer());
        if(key() == id)
        {
            Q_FOREACH(const QByteArray &key, p->list)
            {
                TelegramMessageListItem item = p->items.value(key);
                if(!item.message)
                    continue;

                item.message->setUnread(false);
            }
        }
    }
        break;
    case Update::typeUpdateReadHistoryOutbox:
    {
        const QByteArray &id = TelegramTools::identifier(update.peer());
        if(key() == id)
        {
            Q_FOREACH(const QByteArray &key, p->list)
            {
                TelegramMessageListItem item = p->items.value(key);
                if(!item.message)
                    continue;

                item.message->setUnread(false);
            }
        }
    }
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
    {
        Peer peer(Peer::typePeerChannel);
        peer.setChannelId(update.channelId());
        const QByteArray &id = TelegramTools::identifier(peer);
        if(key() == id)
        {
            Q_FOREACH(const QByteArray &key, p->list)
            {
                TelegramMessageListItem item = p->items.value(key);
                if(!item.message)
                    continue;

                item.message->setUnread(false);
            }
        }
    }
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
    case Update::typeUpdateEditMessage:
    case Update::typeUpdateEditChannelMessage:
    {
        const Message &msg = update.message();
        switch(static_cast<int>(msg.toId().classType()))
        {
        case Peer::typePeerChannel:
            if(msg.toId().channelId() == p->currentPeer->channelId())
                tsdm->insertMessage(msg);
            break;
        case Peer::typePeerChat:
            if(msg.toId().chatId() == p->currentPeer->chatId())
                tsdm->insertMessage(msg);
            break;
        case Peer::typePeerUser:
            if(msg.toId().userId() == p->currentPeer->userId())
                tsdm->insertMessage(msg);
            break;
        }
    }
        break;
    case Update::typeUpdateChannelPinnedMessage:
        break;
    case Update::typeUpdateBotCallbackQuery:
        break;
    case Update::typeUpdateInlineBotCallbackQuery:
        break;
    }
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
    if(i1.message->date() != i1.message->date())
        return i1.message->date() > i2.message->date();
    else
        return i1.message->id() > i2.message->id();
}
