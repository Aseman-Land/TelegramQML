#define DEFINE_DIS \
    QPointer<TelegramUploadHandler> dis = this;

#include "telegramuploadhandler.h"
#include "telegramsharedpointer.h"
#include "telegramtools.h"
#include "telegramengine.h"
#include "telegramshareddatamanager.h"
#include "telegramthumbnailer.h"
#include "telegramenums.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QPointer>
#include <QDateTime>
#include <QMimeDatabase>
#include <QFileInfo>

class TelegramUploadHandlerPrivate
{
public:
    QPointer<TelegramEngine> engine;
    TelegramSharedPointer<InputPeerObject> currentPeer;
    QString text;
    QString file;
    bool silent;
    bool noWebpage;
    int sendFileType;
    int status;
    QByteArray fake_key;

    qint32 totalSize;
    qint32 transfaredSize;

    TelegramSharedPointer<MessageObject> result;
    TelegramSharedPointer<MessageObject> target;
    TelegramSharedPointer<MessageObject> replyTo;
    QPointer<ReplyMarkupObject> replyMarkup;

    TelegramThumbnailer *thumbnailer;

    static QSet<TelegramUploadHandler*> objects;
};

QSet<TelegramUploadHandler*> TelegramUploadHandlerPrivate::objects;

TelegramUploadHandler::TelegramUploadHandler(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramUploadHandlerPrivate;
    p->thumbnailer = 0;
    p->silent = false;
    p->noWebpage = false;
    p->sendFileType = TelegramEnums::SendFileTypeAutoDetect;
    p->status = StatusNone;
    p->totalSize = 0;
    p->transfaredSize = 0;
    TelegramUploadHandlerPrivate::objects.insert(this);
}

void TelegramUploadHandler::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    p->engine = engine;
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramUploadHandler::engine() const
{
    return p->engine;
}

void TelegramUploadHandler::setCurrentPeer(InputPeerObject *peer)
{
    if(p->currentPeer == peer)
        return;

    p->currentPeer = peer;
    Q_EMIT currentPeerChanged();
}

InputPeerObject *TelegramUploadHandler::currentPeer() const
{
    return p->currentPeer;
}

void TelegramUploadHandler::setText(const QString &text)
{
    if(p->text == text)
        return;

    p->text = text;
    Q_EMIT textChanged();
}

QString TelegramUploadHandler::text() const
{
    return p->text;
}

void TelegramUploadHandler::setFile(const QString &file)
{
    if(p->file == file)
        return;

    p->file = file;
    Q_EMIT fileChanged();
}

QString TelegramUploadHandler::file() const
{
    return p->file;
}

void TelegramUploadHandler::setSilent(bool silent)
{
    if(p->silent == silent)
        return;

    p->silent = silent;
    Q_EMIT silentChanged();
}

bool TelegramUploadHandler::silent() const
{
    return p->silent;
}

void TelegramUploadHandler::setNoWebpage(bool noWebpage)
{
    if(p->noWebpage == noWebpage)
        return;

    p->noWebpage = noWebpage;
    Q_EMIT noWebpageChanged();
}

bool TelegramUploadHandler::noWebpage() const
{
    return p->noWebpage;
}

void TelegramUploadHandler::setSendFileType(int type)
{
    if(p->sendFileType == type)
        return;

    p->sendFileType = type;
    Q_EMIT sendFileTypeChanged();
}

int TelegramUploadHandler::sendFileType() const
{
    return p->sendFileType;
}

void TelegramUploadHandler::setReplyTo(MessageObject *message)
{
    if(p->replyTo == message)
        return;

    p->replyTo = message;
    Q_EMIT replyToChanged();
}

MessageObject *TelegramUploadHandler::replyTo() const
{
    return p->replyTo;
}

void TelegramUploadHandler::setReplyMarkup(ReplyMarkupObject *markup)
{
    if(p->replyMarkup == markup)
        return;

    p->replyMarkup = markup;
    Q_EMIT replyMarkupChanged();
}

ReplyMarkupObject *TelegramUploadHandler::replyMarkup() const
{
    return p->replyMarkup;
}

void TelegramUploadHandler::setStatus(int status)
{
    if(p->status == status)
        return;

    p->status = status;
    Q_EMIT statusChanged();
}

void TelegramUploadHandler::setFakeKey(const QByteArray &fakeKey)
{
    if(p->fake_key == fakeKey)
        return;

    p->fake_key = fakeKey;
    Q_EMIT fakeKeyChanged();
}

int TelegramUploadHandler::status() const
{
    return p->status;
}

void TelegramUploadHandler::setTarget(MessageObject *object)
{
    if(p->target == object)
        return;

    p->target = object;
    Q_EMIT targetChanged();
}

MessageObject *TelegramUploadHandler::target() const
{
    return p->target;
}

MessageObject *TelegramUploadHandler::result() const
{
    return p->result;
}

void TelegramUploadHandler::setResult(const Message &message)
{
    if(p->result && p->result->operator ==(message))
        return;
    TelegramSharedDataManager *tsdm = p->engine->sharedData();
    if(!tsdm)
        p->result = new MessageObject(message);
    else
        p->result = tsdm->insertMessage(message);

    Q_EMIT resultChanged();
}

QByteArray TelegramUploadHandler::fakeKey() const
{
    return p->fake_key;
}

qint32 TelegramUploadHandler::transfaredSize() const
{
    return p->transfaredSize;
}

void TelegramUploadHandler::setTransfaredSize(qint32 size)
{
    if(p->transfaredSize == size)
        return;

    p->transfaredSize = size;
    Q_EMIT transfaredSizeChanged();
}

qint32 TelegramUploadHandler::totalSize() const
{
    return p->totalSize;
}

void TelegramUploadHandler::setTotalSize(qint32 size)
{
    if(p->totalSize == size)
        return;

    p->totalSize = size;
    Q_EMIT totalSizeChanged();
}

QList<TelegramUploadHandler *> TelegramUploadHandler::getItems(TelegramEngine *engine, InputPeerObject *peer)
{
    QList<TelegramUploadHandler*> result;
    Q_FOREACH(TelegramUploadHandler *item, TelegramUploadHandlerPrivate::objects)
        if(item->p->engine == engine && item->p->currentPeer == peer)
            result << item;
    return result;
}

QList<TelegramUploadHandler *> TelegramUploadHandler::getItems()
{
    return getItems(p->engine, p->currentPeer);
}

bool TelegramUploadHandler::send()
{
    if(p->status != StatusNone && p->status != StatusError)
        return false;

    setFakeKey(QByteArray());
    if(p->file.isEmpty())
        return sendMessage();
    else
        return sendFile();
}

void TelegramUploadHandler::cancel()
{
    if(p->status != StatusUploading)
        return;
    if(!p->target)
        return;
}

bool TelegramUploadHandler::sendMessage()
{
    Message newMsg = createNewMessage();
    if(newMsg.classType() == Message::typeMessageEmpty)
        return false;

    newMsg.setMessage(p->text);
    if(p->replyTo)
        newMsg.setReplyToMsgId(p->replyTo->id());
    if(p->replyMarkup)
        newMsg.setReplyMarkup(p->replyMarkup->core());

    setResult(newMsg);
    setFakeKey(TelegramTools::identifier(newMsg));
    setStatus(StatusSending);

    DEFINE_DIS;
    Telegram *tg = p->engine->telegram();
    if(!tg) return false;

    const bool broadcast = (p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel);

    tg->messagesSendMessage(p->noWebpage, broadcast, p->silent, false, p->currentPeer->core(), p->replyTo?p->replyTo->id():0,
                            p->text, TelegramTools::generateRandomId(), p->replyMarkup?p->replyMarkup->core():ReplyMarkup::null,
                            QList<MessageEntity>(), [this, dis, newMsg](TG_MESSAGES_SEND_MESSAGE_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis || !p->engine) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        Message message = newMsg;
        message.setId(result.id());
        message.setFwdFrom(result.fwdFrom());
        message.setDate(result.date());
        message.setMedia(result.media());
        message.setUnread(result.unread());
        message.setOut(result.out());
        message.setEntities(result.entities());

        setResult(message);
        setStatus(StatusDone);
    });

    return true;
}

bool TelegramUploadHandler::sendFile()
{
    bool res = false;
    int type = p->sendFileType;
    if(type == TelegramEnums::SendFileTypeAutoDetect)
    {
        QMimeDatabase mdb;
        QMimeType t = mdb.mimeTypeForFile(p->file);
        if(t.name().contains("webp") || p->file.right(5) == ".webp")
            type = TelegramEnums::SendFileTypeSticker;
        else
        if(!t.name().contains("gif") && t.name().contains("image/"))
            type = TelegramEnums::SendFileTypePhoto;
        else
        if(t.name().contains("video/"))
            type = TelegramEnums::SendFileTypeVideo;
        else
        if(t.name().contains("audio/"))
            type = TelegramEnums::SendFileTypeAudio;
        else
            type = TelegramEnums::SendFileTypeDocument;
    }

    switch(type)
    {
    case TelegramEnums::SendFileTypeDocument:
        res = sendDocument();
        break;
    case TelegramEnums::SendFileTypeSticker:
        break;
    case TelegramEnums::SendFileTypeVideo:
        break;
    case TelegramEnums::SendFileTypePhoto:
        break;
    case TelegramEnums::SendFileTypeAudio:
        break;
    }

    return res;
}

bool TelegramUploadHandler::sendDocument()
{
    Message newMsg = createNewMessage();
    if(newMsg.classType() == Message::typeMessageEmpty)
        return false;

    if(p->replyTo)
        newMsg.setReplyToMsgId(p->replyTo->id());
    if(p->replyMarkup)
        newMsg.setReplyMarkup(p->replyMarkup->core());

    setResult(newMsg);
    setFakeKey(TelegramTools::identifier(newMsg));
    setStatus(StatusSending);

    if(!p->thumbnailer)
        p->thumbnailer = new TelegramThumbnailer(this);

    QString thumbnail = p->thumbnailer->getThumbPath(p->engine->tempPath(), p->file);

    DEFINE_DIS;
    p->thumbnailer->createThumbnail(p->file, thumbnail, [this, dis, newMsg, thumbnail](){
        if(!dis || !p->engine) return;
        Telegram *tg = p->engine->telegram();
        if(!tg) return;

        const bool broadcast = (p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel);

        tg->messagesSendDocument(p->currentPeer->core(), TelegramTools::generateRandomId(), p->file,
                                 QFileInfo::exists(thumbnail)?thumbnail:"", false,
                                 p->replyTo?p->replyTo->id():0, p->replyMarkup?p->replyMarkup->core():ReplyMarkup::null,
                                 broadcast, p->silent, false, [this, dis, newMsg](TG_UPLOAD_SEND_FILE_CUSTOM_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            switch(static_cast<int>(result.classType()))
            {
            case UploadSendFile::typeUploadSendFileCanceled:
            case UploadSendFile::typeUploadSendFileFinished:
            case UploadSendFile::typeUploadSendFileEmpty:
            {
                const UpdatesType &upd = result.updates();
                Message message = newMsg;
                message.setId(upd.id());
                message.setFwdFrom(upd.fwdFrom());
                message.setDate(upd.date());
                message.setMedia(upd.media());
                message.setUnread(upd.unread());
                message.setEntities(upd.entities());

                setResult(message);
                setStatus(StatusDone);
            }
                break;

            case UploadSendFile::typeUploadSendFileProgress:
                setTotalSize(result.totalSize());
                setTransfaredSize(result.uploaded());
                break;
            }
        });
    });

    return true;
}

QByteArray TelegramUploadHandler::identifier() const
{
    return p->currentPeer? TelegramTools::identifier(p->currentPeer->core()) : QByteArray();
}

Message TelegramUploadHandler::createNewMessage()
{
    Message msg;
    if(!p->engine || !p->engine->our() || !p->engine->telegram() || !p->currentPeer)
        return msg;

    User user = p->engine->our()->user()->core();

    msg.setFromId(user.id());
    msg.setOut(true);
    msg.setUnread(false);
    msg.setToId(TelegramTools::inputPeerPeer(p->currentPeer->core()));
    msg.setClassType(Message::typeMessage);
    msg.setDate(QDateTime::currentDateTime().toTime_t());
    msg.setId(TelegramTools::generateRandomId());

    return msg;
}

TelegramUploadHandler::~TelegramUploadHandler()
{
    TelegramUploadHandlerPrivate::objects.remove(this);
    delete p;
}

