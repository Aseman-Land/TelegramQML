#define DEFINE_DIS \
    QPointer<TelegramMessageFetcher> dis = this;

#include "telegrammessagefetcher.h"
#include "telegramsharedpointer.h"
#include "telegramshareddatamanager.h"
#include "telegramtools.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QPointer>

class TelegramMessageFetcherPrivate
{
public:
    QPointer<TelegramEngine> engine;

    TelegramSharedPointer<InputPeerObject> inputPeer;
    TelegramSharedPointer<TelegramTypeQObject> rootPeer;

    TelegramSharedPointer<MessageObject> result;
    qint32 messageId;
};

TelegramMessageFetcher::TelegramMessageFetcher(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramMessageFetcherPrivate;
    p->messageId = 0;
}

void TelegramMessageFetcher::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramMessageFetcher::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramMessageFetcher::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramMessageFetcher::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramMessageFetcher::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramMessageFetcher::engine() const
{
    return p->engine;
}

void TelegramMessageFetcher::setInputPeer(InputPeerObject *inputPeer)
{
    if(p->inputPeer == inputPeer)
        return;

    p->inputPeer = inputPeer;
    p->rootPeer = TelegramTools::objectRoot(inputPeer);

    refresh();
    Q_EMIT inputPeerChanged();
}

InputPeerObject *TelegramMessageFetcher::inputPeer() const
{
    return p->inputPeer;
}

void TelegramMessageFetcher::setMessageId(qint32 msgId)
{
    if(p->messageId == msgId)
        return;

    p->messageId = msgId;
    refresh();
    Q_EMIT messageIdChanged();
}

qint32 TelegramMessageFetcher::messageId() const
{
    return p->messageId;
}

MessageObject *TelegramMessageFetcher::result() const
{
    return p->result;
}

void TelegramMessageFetcher::refresh()
{
    if(!p->engine || !p->engine->telegram() || p->engine->state() != TelegramEngine::AuthLoggedIn
        || !p->messageId || !p->inputPeer)
    {
        clean();
        return;
    }

    clean();

    Telegram *tg = p->engine->telegram();
    DEFINE_DIS
    tg->messagesGetMessages( QList<qint32>()<<p->messageId, [this, dis](TG_MESSAGES_GET_MESSAGES_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        if(!p->engine || !p->engine->sharedData()) return;

        TelegramSharedDataManager *tsdm = p->engine->sharedData();
        Q_FOREACH(const Message &msg, result.messages())
            if(msg.id() == p->messageId)
            {
                p->result = tsdm->insertMessage(msg);
                Q_EMIT resultChanged();
                break;
            }
    });
}

void TelegramMessageFetcher::clean()
{
    p->result = 0;
    Q_EMIT resultChanged();
}

TelegramMessageFetcher::~TelegramMessageFetcher()
{
    delete p;
}

