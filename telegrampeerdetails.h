#ifndef TELEGRAMPEERDETAILS_H
#define TELEGRAMPEERDETAILS_H

#include "tqobject.h"

#include <QDateTime>
#include <QJSValue>

class Peer;
class User;
class Update;
class Chat;
class UserFullObject;
class ChatFullObject;
class TelegramEngine;
class PeerObject;
class TelegramPeerDetailsPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramPeerDetails : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(PeerObject* peer READ peer WRITE setPeer NOTIFY peerChanged)
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)

    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isUser READ isUser NOTIFY isUserChanged)
    Q_PROPERTY(bool isChannel READ isChannel NOTIFY isChannelChanged)

    Q_PROPERTY(QString key READ key NOTIFY keyChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(int participantsCount READ participantsCount NOTIFY participantsCountChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)

    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(bool blocked READ blocked WRITE setBlocked NOTIFY blockedChanged)

    Q_PROPERTY(UserFullObject* userFull READ userFull NOTIFY userFullChanged)
    Q_PROPERTY(ChatFullObject* chatFull READ chatFull NOTIFY chatFullChanged)
    Q_PROPERTY(QVariantList chatUsers READ chatUsers NOTIFY chatUsersChanged)

public:
    TelegramPeerDetails(QObject *parent = 0);
    ~TelegramPeerDetails();

    void setPeer(PeerObject *peer);
    PeerObject *peer() const;

    bool isChat() const;
    bool isUser() const;
    bool isChannel() const;

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    QJSValue dateConvertorMethod() const;
    void setDateConvertorMethod(const QJSValue &method);

    QString key() const;
    QString displayName() const;
    int participantsCount() const;
    QString statusText() const;
    QString phoneNumber() const;
    QString username() const;

    void setMute(bool mute);
    bool mute() const;

    void setBlocked(bool blocked);
    bool blocked() const;

    UserFullObject *userFull() const;
    ChatFullObject *chatFull() const;
    QVariantList chatUsers() const;

Q_SIGNALS:
    void peerChanged();
    void engineChanged();
    void dateConvertorMethodChanged();

    void isChatChanged();
    void isUserChanged();
    void isChannelChanged();

    void keyChanged();
    void displayNameChanged();
    void participantsCountChanged();
    void statusTextChanged();
    void phoneNumberChanged();
    void usernameChanged();

    void muteChanged();
    void blockedChanged();

    void userFullChanged();
    void chatFullChanged();
    void chatUsersChanged();

public Q_SLOTS:

protected:
    void refresh();
    void initTelegram();

    void connectChatSignals(class ChatObject *chat, bool dc = false);
    void connectUserSignals(class UserObject *user, bool dc = false);
    void connectDialogSignals(class DialogObject *dialog, bool dc = false);

    void insertChatFull(const class MessagesChatFull &result);

    QString convetDate(const QDateTime &td) const;

    virtual void onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    virtual void onUpdates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);
    void insertUpdate(const Update &update);

private:
    TelegramPeerDetailsPrivate *p;
};

#endif // TELEGRAMPEERDETAILS_H
