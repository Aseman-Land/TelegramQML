#ifndef TELEGRAMENGINE_H
#define TELEGRAMENGINE_H

#include <QObject>

#include "telegramqml_macros.h"
#include "tqobject.h"

class Telegram;
class TelegramHost;
class TelegramApp;
class TelegramSharedDataManager;
class TelegramEnginePrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramEngine : public TqObject
{
    Q_OBJECT
    Q_ENUMS(AuthState)
    Q_ENUMS(LogLevel)

    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString configDirectory READ configDirectory WRITE setConfigDirectory NOTIFY configDirectoryChanged)
    Q_PROPERTY(TelegramApp* app READ app WRITE setApp NOTIFY appChanged)
    Q_PROPERTY(TelegramHost* host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(int logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)

public:
    enum AuthState {
        AuthUnknown,
        AuthInitializing,
        AuthNeeded,
        AuthLoggedIn
    };

    enum LogLevel {
        LogLevelClean,
        LogLevelUseful,
        LogLevelFull
    };

    TelegramEngine(QObject *parent = 0);
    ~TelegramEngine();

    void setPhoneNumber(const QString &phoneNumber);
    QString phoneNumber() const;

    void setConfigDirectory(const QString &configDirectory);
    QString configDirectory() const;

    void setApp(TelegramApp *app);
    TelegramApp *app() const;

    void setHost(TelegramHost *host);
    TelegramHost *host() const;

    void setTimeout(int ms);
    int timeout() const;

    void setLogLevel(int level);
    int logLevel() const;

    virtual bool isValid() const;
    Telegram *telegram() const;
    TelegramSharedDataManager *sharedData() const;

    int state() const;

public Q_SLOTS:

Q_SIGNALS:
    void authLoggedIn();
    void authNeeded();

    void phoneNumberChanged();
    void configDirectoryChanged();
    void appChanged();
    void hostChanged();
    void timeoutChanged();
    void stateChanged();
    void telegramChanged();
    void logLevelChanged();

protected:
    void tryInit();
    void clean();
    void itemsChanged_slt();

private:
    TelegramEnginePrivate *p;
};

#endif // TELEGRAMENGINE_H
