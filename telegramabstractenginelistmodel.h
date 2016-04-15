#ifndef TELEGRAMABSTRACTENGINELISTMODEL_H
#define TELEGRAMABSTRACTENGINELISTMODEL_H

#include "telegramengine.h"
#include "telegramqml_global.h"
#include "telegramabstractlistmodel.h"
#include "telegramtools.h"

#include <QStringList>
#include <QPointer>

#include <functional>

class UpdatesType;
class Peer;
class User;
class Update;
class Chat;
class UpdatesState;
class UpdatesDifference;
class UpdatesChannelDifference;
class MessageFwdHeader;
class Message;
class SecretChatMessage;
class TELEGRAMQMLSHARED_EXPORT TelegramAbstractEngineListModel : public TelegramAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorChanged)
    Q_PROPERTY(qint32 errorCode READ errorCode NOTIFY errorChanged)

public:
    TelegramAbstractEngineListModel(QObject *parent = 0);
    virtual ~TelegramAbstractEngineListModel();

    typedef std::function<void ()> Callback;

    TelegramEngine *engine() const { return mEngine; }
    void setEngine(TelegramEngine *engine);

    QString errorText() const { return mErrorText; }
    qint32 errorCode() const { return mErrorCode; }

public Q_SLOTS:
    void startTimer(int ms, Callback callback);

Q_SIGNALS:
    void engineChanged();
    void errorChanged();
    void countChanged();

protected:
    virtual void refresh() = 0;
    virtual void clean() = 0;
    QPointer<TelegramEngine> mEngine;

    void setError(const QString &errorText, qint32 errorCode) {
        mErrorText = TelegramTools::convertErrorToText(errorText);
        mErrorCode = errorCode;
        Q_EMIT errorChanged();
    }

    virtual void onUpdatesGetDifferenceAnswer(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);
    virtual void onUpdatesGetStateAnswer(qint64 msgId, const UpdatesState &result);
    virtual void onUpdatesGetChannelDifferenceAnswer(qint64 msgId, const UpdatesChannelDifference &result);
    virtual void onUpdateSecretChatMessage(const SecretChatMessage &secretChatMessage, qint32 qts);
    virtual void onUpdates(const UpdatesType &update);

    virtual void timerEvent(QTimerEvent *e);

private:
    void connectTelegram();

private:
    QString mErrorText;
    qint32 mErrorCode;
    QPointer<Telegram> mTelegram;
    QHash<qint32, Callback> mTimers;
};

#endif // TELEGRAMABSTRACTENGINELISTMODEL_H
