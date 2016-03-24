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
#include "telegramprofilemanagermodel.h"

#include <telegram.h>

#include <QPointer>
#include <QDir>
#include <QTimer>
#include <QCoreApplication>

class TelegramEnginePrivate
{
public:
    QPointer<TelegramSharedDataManager> sharedData;
    QPointer<Telegram> telegram;
    QPointer<TelegramApp> app;
    QPointer<TelegramHost> host;
    QPointer<TelegramProfileManagerModel> profileManager;
    TelegramSharedPointer<UserFullObject> our;

    QString phoneNumber;
    QString configDirectory;
    qint32 timeout;
    qint32 logLevel;
    qint32 state;
    QString tempPath;

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
    p->tempPath = QDir::tempPath() + "/" + QCoreApplication::applicationName();

    setApp(new TelegramApp(this));
    setHost(new TelegramHost(this));

    connect(this, &TelegramEngine::itemsChanged, this, &TelegramEngine::itemsChanged_slt);
}

void TelegramEngine::setPhoneNumber(const QString &phoneNumber)
{
    PROPERTY_SET_TRY(phoneNumber);
}

QString TelegramEngine::phoneNumber() const
{
    PROPERTY_GET_TRY(phoneNumber);
}

void TelegramEngine::setConfigDirectory(const QString &configDirectory)
{
    PROPERTY_SET_TRY(configDirectory);
}

QString TelegramEngine::configDirectory() const
{
    PROPERTY_GET_TRY(configDirectory);
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

void TelegramEngine::setProfileManager(TelegramProfileManagerModel *manager)
{
    if(p->profileManager == manager)
        return;

    p->profileManager = manager;
    Q_EMIT profileManagerChanged();
}

TelegramProfileManagerModel *TelegramEngine::profileManager() const
{
    return p->profileManager;
}

void TelegramEngine::setTimeout(qint32 ms)
{
    if(p->timeout == ms)
        return;

    p->timeout = ms;
    if(p->telegram)
        p->telegram->setTimeOut(p->timeout);

    Q_EMIT timeoutChanged();
}

qint32 TelegramEngine::timeout() const
{
    return p->timeout;
}

void TelegramEngine::setLogLevel(qint32 level)
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

qint32 TelegramEngine::logLevel() const
{
    return p->logLevel;
}

void TelegramEngine::setTempPath(const QString &tempPath)
{
    PROPERTY_SET_TRY(tempPath)
}

QString TelegramEngine::tempPath() const
{
    PROPERTY_GET_TRY(tempPath);
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

UserFullObject *TelegramEngine::our() const
{
    return p->our;
}

qint32 TelegramEngine::state() const
{
    return p->state;
}

void TelegramEngine::setState(qint32 state)
{
    if(p->state == state)
        return;

    p->state = state;
    if(p->state == AuthLoggedIn && p->profileManager)
        p->profileManager->add(p->phoneNumber, false, this);

    Q_EMIT stateChanged();
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

        p->telegram = new Telegram(p->host->hostAddress(), p->host->hostPort(), p->host->hostDcId(),
                                   p->app->appId(), p->app->appHash(),
                                   p->phoneNumber, p->configDirectory, publicKeyPath);
        p->telegram->setTimeOut(p->timeout);

        connect(p->telegram.data(), &Telegram::authNeeded, this, [this](){
            setState(AuthNeeded);
            Q_EMIT authNeeded();
        });

        connect(p->telegram.data(), &Telegram::authLoggedIn, this, [this](){
            setState(AuthFetchingOurDetails);
            InputUser user(InputUser::typeInputUserSelf);
            p->telegram->usersGetFullUser(user, [this](TG_USERS_GET_FULL_USER_CALLBACK){
                Q_UNUSED(msgId)
                Q_UNUSED(error)
                p->our = new UserFullObject(result);
                setState(AuthLoggedIn);
                Q_EMIT ourChanged();
                Q_EMIT authLoggedIn();
            });
        });

        connect(p->telegram.data(), &Telegram::authLogOutAnswer, this, [this](qint64, bool result){
            if(!result)
                return;
            if(p->profileManager)
                p->profileManager->remove(p->phoneNumber);
            setPhoneNumber("");
        });

        setState(AuthInitializing);
        p->telegram->init(15000);

        Q_EMIT telegramChanged();
    });
}

void TelegramEngine::clean()
{
    if(!p->telegram)
        return;

    delete p->telegram;

    setState(AuthUnknown);
    Q_EMIT telegramChanged();
}

void TelegramEngine::itemsChanged_slt()
{
    Q_FOREACH(QObject *obj, _items)
    {
        if(qobject_cast<TelegramAuthenticate*>(obj))
            qobject_cast<TelegramAuthenticate*>(obj)->setEngine(this);
    }
}

void TelegramEngine::cleanTemp()
{
    if(p->tempPath.isEmpty())
        return;
}

TelegramEngine::~TelegramEngine()
{
    cleanTemp();
    delete p;
}
