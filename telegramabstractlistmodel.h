#ifndef TELEGRAMABSTRACTLISTMODEL_H
#define TELEGRAMABSTRACTLISTMODEL_H

#include "telegramengine.h"
#include "telegramqml_global.h"

#include <QAbstractListModel>
#include <QStringList>
#include <QPointer>

#include <functional>

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
class TELEGRAMQMLSHARED_EXPORT TelegramAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorChanged)
    Q_PROPERTY(qint32 errorCode READ errorCode NOTIFY errorChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    TelegramAbstractListModel(QObject *parent = 0);
    virtual ~TelegramAbstractListModel();

    typedef std::function<void ()> Callback;

    Q_INVOKABLE QStringList roles() const;

    TelegramEngine *engine() const { return mEngine; }
    void setEngine(TelegramEngine *engine);

    QString errorText() const { return mErrorText; }
    qint32 errorCode() const { return mErrorCode; }

    int rowCount(const QModelIndex &) const { return count(); }
    virtual int count() const = 0;

public Q_SLOTS:
    QVariant get(int index, int role) const;
    QVariant get(int index, const QString &roleName) const;
    QVariantMap get(int index) const;

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
        mErrorText = errorText;
        mErrorCode = errorCode;
        Q_EMIT errorChanged();
    }

    virtual void onUpdatesGetDifferenceAnswer(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);
    virtual void onUpdatesGetStateAnswer(qint64 msgId, const UpdatesState &result);
    virtual void onUpdatesGetChannelDifferenceAnswer(qint64 msgId, const UpdatesChannelDifference &result);
    virtual void onUpdateSecretChatMessage(const SecretChatMessage &secretChatMessage, qint32 qts);
    virtual void onUpdatesTooLong();
    virtual void onUpdateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShort(const Update &update, qint32 date);
    virtual void onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    virtual void onUpdates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

    virtual void timerEvent(QTimerEvent *e);

private:
    void connectTelegram();

private:
    QString mErrorText;
    qint32 mErrorCode;
    QPointer<Telegram> mTelegram;
    QHash<qint32, Callback> mTimers;
};

#endif // TELEGRAMABSTRACTLISTMODEL_H
