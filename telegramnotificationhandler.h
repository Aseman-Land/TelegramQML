#ifndef TELEGRAMNOTIFICATIONHANDLER_H
#define TELEGRAMNOTIFICATIONHANDLER_H

#include "tqobject.h"
#include "telegramqml_global.h"

class Peer;
class User;
class Update;
class Chat;
class MessageFwdHeader;
class TelegramEngine;
class TelegramNotificationHandlerPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramNotificationHandler : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)

public:
    TelegramNotificationHandler(QObject *parent = 0);
    ~TelegramNotificationHandler();

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

Q_SIGNALS:
    void engineChanged();
    void newMessage(const QString &title, const QString &message, const QString &peerKey);

public Q_SLOTS:

protected:
    virtual void onUpdateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShort(const Update &update, qint32 date);
    virtual void onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    virtual void onUpdates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

    void insertUpdate(const Update &update);
    void refresh();

private:
    TelegramNotificationHandlerPrivate *p;
};

#endif // TELEGRAMNOTIFICATIONHANDLER_H
