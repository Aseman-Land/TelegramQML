#define PROPERTY_SET_TRY(NAME) \
    if(p->NAME == NAME) return; \
    p->NAME = NAME; \
    tryInit(); \
    Q_EMIT NAME##Changed();

#define PROPERTY_GET_TRY(NAME) \
    return p->NAME;

#include "telegramengine.h"
#include "telegramapp.h"
#include "telegramhost.h"
#include "telegramauthenticate.h"
#include "telegramshareddatamanager.h"

#include <telegram.h>

#include <QPointer>
#include <QDir>
#include <QTimer>

class TelegramEnginePrivate
{
public:
    QPointer<TelegramSharedDataManager> sharedData;
    QPointer<Telegram> telegram;
    QPointer<TelegramApp> app;
    QPointer<TelegramHost> host;

    QString phoneNumber;
    QString configDirectory;
    int timeout;
    int logLevel;
    int state;

    QTimer *initTimer;
};

TelegramEngine::TelegramEngine(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramEnginePrivate;
    p->timeout = 15000;
    p->state = AuthUnknown;
    p->logLevel = LogLevelFull;
    p->initTimer = 0;
    p->sharedData = new TelegramSharedDataManager(this);

    setApp(new TelegramApp(this));
    setHost(new TelegramHost(this));

    connect(this, &TelegramEngine::itemsChanged, this, &TelegramEngine::itemsChanged_slt);
}

void TelegramEngine::setPhoneNumber(const QString &phoneNumber)
{
    PROPERTY_SET_TRY(phoneNumber)
}

QString TelegramEngine::phoneNumber() const
{
    PROPERTY_GET_TRY(phoneNumber)
}

void TelegramEngine::setConfigDirectory(const QString &configDirectory)
{
    PROPERTY_SET_TRY(configDirectory)
}

QString TelegramEngine::configDirectory() const
{
    PROPERTY_GET_TRY(configDirectory)
}

void TelegramEngine::setApp(TelegramApp *app)
{
    if(p->app == app)
        return;
    if(p->app)
        disconnect(p->app.data(), &TelegramApp::isValidChanged, this, &TelegramEngine::tryInit);

    p->app = app;
    if(p->app)
        connect(p->app.data(), &TelegramApp::isValidChanged, this, &TelegramEngine::tryInit);

    tryInit();
    Q_EMIT appChanged();
}

TelegramApp *TelegramEngine::app() const
{
    return p->app;
}

void TelegramEngine::setHost(TelegramHost *host)
{
    if(p->host == host)
        return;
    if(p->host)
        disconnect(p->host.data(), &TelegramHost::isValidChanged, this, &TelegramEngine::tryInit);

    p->host = host;
    if(p->host)
        connect(p->host.data(), &TelegramHost::isValidChanged, this, &TelegramEngine::tryInit);

    tryInit();
    Q_EMIT hostChanged();
}

TelegramHost *TelegramEngine::host() const
{
    return p->host;
}

void TelegramEngine::setTimeout(int ms)
{
    if(p->timeout == ms)
        return;

    p->timeout = ms;
    if(p->telegram)
        p->telegram->setTimeOut(p->timeout);

    Q_EMIT timeoutChanged();
}

int TelegramEngine::timeout() const
{
    return p->timeout;
}

void TelegramEngine::setLogLevel(int level)
{
    if(p->logLevel == level)
        return;

    p->logLevel = level;
    switch(level)
    {
    case LogLevelClean:
        qputenv("QT_LOGGING_RULES", "tg.*=false");
        break;

    case LogLevelUseful:
        qputenv("QT_LOGGING_RULES", "tg.core.settings=false\n"
                                    "tg.core.outboundpkt=false\n"
                                    "tg.core.inboundpkt=false");
        break;

    case LogLevelFull:
        qputenv("QT_LOGGING_RULES", "");
        break;
    }

    Q_EMIT logLevelChanged();
}

int TelegramEngine::logLevel() const
{
    return p->logLevel;
}

bool TelegramEngine::isValid() const
{
    return p->app && p->app->isValid() &&
            p->host && p->host->isValid() &&
            !p->phoneNumber.isEmpty() &&
            !p->configDirectory.isEmpty() && QDir().mkpath(p->configDirectory);
}

Telegram *TelegramEngine::telegram() const
{
    return p->telegram;
}

TelegramSharedDataManager *TelegramEngine::sharedData() const
{
    return p->sharedData;
}

int TelegramEngine::state() const
{
    return p->state;
}

void TelegramEngine::tryInit()
{
    if(p->initTimer)
        delete p->initTimer;

    p->initTimer = new QTimer(this);
    p->initTimer->setInterval(200);
    p->initTimer->setSingleShot(true);
    p->initTimer->start();

    connect(p->initTimer, &QTimer::timeout, [this](){
        clean();
        if(!isValid())
            return;

        QString publicKeyPath = p->host->publicKey().toLocalFile();
        if(publicKeyPath.isEmpty())
            publicKeyPath = p->host->publicKey().toString();
        if(publicKeyPath.left(4) == "qrc:")
            publicKeyPath = publicKeyPath.mid(3);

        p->state = AuthInitializing;
        p->telegram = new Telegram(p->host->hostAddress(), p->host->hostPort(), p->host->hostDcId(),
                                   p->app->appId(), p->app->appHash(),
                                   p->phoneNumber, p->configDirectory, publicKeyPath);
        p->telegram->setTimeOut(p->timeout);

        connect(p->telegram.data(), &Telegram::authNeeded, [this](){
            p->state = AuthNeeded;
            Q_EMIT stateChanged();
            Q_EMIT authNeeded();
        });

        connect(p->telegram.data(), &Telegram::authLoggedIn, [this](){
            p->state = AuthLoggedIn;
            Q_EMIT stateChanged();
            Q_EMIT authLoggedIn();
        });

        p->telegram->init(15000);

        Q_EMIT telegramChanged();
        Q_EMIT stateChanged();
    });
}

void TelegramEngine::clean()
{
    if(!p->telegram)
        return;

    delete p->telegram;

    p->state = AuthUnknown;

    Q_EMIT telegramChanged();
    Q_EMIT stateChanged();
}

void TelegramEngine::itemsChanged_slt()
{
    Q_FOREACH(QObject *obj, _items)
    {
        if(qobject_cast<TelegramAuthenticate*>(obj))
            qobject_cast<TelegramAuthenticate*>(obj)->setEngine(this);
    }
}

TelegramEngine::~TelegramEngine()
{
    delete p;
}
