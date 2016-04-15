#define DEFINE_DIS \
    QPointer<TelegramAuthenticate> dis = this;

#include "telegramauthenticate.h"
#include "telegramengine.h"

#include <telegram.h>

#include <QPointer>

class TelegramAuthenticatePrivate
{
public:
    QPointer<TelegramEngine> engine;
    int state;
    int callTimeout;
    int remainingTime;

    QString signup_firstName;
    QString signup_lastName;

    QTimer *remainTimer;
};

TelegramAuthenticate::TelegramAuthenticate(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramAuthenticatePrivate;
    p->state = AuthUnknown;
    p->callTimeout = 0;
    p->remainingTime = 0;
    p->remainTimer = 0;
}

TelegramEngine *TelegramAuthenticate::engine() const
{
    return p->engine;
}

void TelegramAuthenticate::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;
    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramAuthenticate::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramAuthenticate::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramAuthenticate::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramAuthenticate::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

int TelegramAuthenticate::state() const
{
    return p->state;
}

int TelegramAuthenticate::callTimeout() const
{
    return p->callTimeout;
}

int TelegramAuthenticate::remainingTime() const
{
    return p->remainingTime;
}

void TelegramAuthenticate::signUp(const QString &firstName, const QString &lastName)
{
    if(p->state != AuthSignUpNeeded)
    {
        qDebug() << "Authenticate Error: You can only call signUp method, when state is AuthSignUpNeeded.";
        return;
    }

    p->signup_firstName = firstName.trimmed();
    p->signup_lastName = lastName.trimmed();
    requestCode();
}

void TelegramAuthenticate::signIn(const QString &code)
{
    if(p->state != AuthCodeRquested && p->state != AuthCodeRequestingError)
    {
        qDebug() << "Authenticate Error: You can only call signIn method, when state is AuthCodeRquested.";
        return;
    }
    if(code.trimmed().isEmpty())
    {
        qDebug() << "Authenticate Error: you can sign-in using empty code.";
        return;
    }

    DEFINE_DIS;
    TelegramCore::Callback<AuthAuthorization> callback = [this, dis](TG_AUTH_SIGN_IN_CALLBACK) {
        Q_UNUSED(msgId)
        Q_UNUSED(result)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            switchState(AuthLoggingInError);
        }
    };

    switchState(AuthLoggingIn);
    Telegram *tg = p->engine->telegram();
    if(!p->signup_firstName.isEmpty() && !p->signup_lastName.isEmpty())
        tg->authSignUp(code.trimmed(), p->signup_firstName, p->signup_lastName, callback);
    else
        tg->authSignIn(code.trimmed(), callback);
}

void TelegramAuthenticate::refresh()
{
    if(!p->engine || !p->engine->telegram())
    {
        clean();
        return;
    }

    Telegram *tg = p->engine->telegram();

    DEFINE_DIS;
    if(tg)
        connect(tg, &Telegram::authLoggedIn, [this, dis](){
            if(!dis) return;
            switchState(AuthLoggedIn);
            Q_EMIT authLoggedIn();
        });

    if(!tg)
        switchState(AuthUnknown);
    else
    if(tg->isLoggedIn() || p->engine->state() == TelegramEngine::AuthLoggedIn)
        switchState(AuthLoggedIn);
    else
    if(p->engine->state() != TelegramEngine::AuthNeeded)
        switchState(AuthUnknown);
    else
    {
        switchState(AuthCheckingPhone);
        tg->authCheckPhone([this, dis](TG_AUTH_CHECK_PHONE_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                switchState(AuthCheckingPhoneError);
            }
            else
            if(result.phoneRegistered())
                requestCode();
            else
                switchState(AuthSignUpNeeded);
        });
    }
}

void TelegramAuthenticate::clean()
{
    p->signup_firstName.clear();
    p->signup_lastName.clear();
    p->callTimeout = 0;
    p->remainingTime = 0;

    switchState(AuthUnknown);
    Q_EMIT callTimeoutChanged();
    Q_EMIT remainingTimeChanged();
}

void TelegramAuthenticate::startRemainingTimer(int time)
{
    if(p->remainingTime == time)
        return;

    if(p->remainTimer)
        delete p->remainTimer;

    p->remainTimer = 0;
    p->remainingTime = time;
    if(!time)
    {
        Q_EMIT remainingTimeChanged();
        return;
    }

    p->remainTimer = new QTimer(this);
    p->remainTimer->setSingleShot(false);
    p->remainTimer->setInterval(1000);
    p->remainTimer->start();

    connect(p->remainTimer, &QTimer::timeout, [this](){
        p->remainingTime--;
        Q_EMIT remainingTimeChanged();
        if(p->remainingTime == 0)
            p->remainTimer->deleteLater();
    });
}

void TelegramAuthenticate::requestCode()
{
    Telegram *tg = p->engine->telegram();
    DEFINE_DIS;
    tg->authSendCode([this, dis](TG_AUTH_SEND_CODE_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            switchState(AuthCodeRequestingError);
        } else {
            p->callTimeout = result.timeout();
            startRemainingTimer(p->callTimeout);
            Q_EMIT callTimeoutChanged();
            switchState(AuthCodeRquested);
        }
    });

    switchState(AuthCodeRequesting);
}

void TelegramAuthenticate::switchState(int state)
{
    if(p->state == state)
        return;

    p->state = state;
    Q_EMIT stateChanged();
}

TelegramAuthenticate::~TelegramAuthenticate()
{
    delete p;
}

