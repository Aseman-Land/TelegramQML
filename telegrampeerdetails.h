#ifndef TELEGRAMPEERDETAILS_H
#define TELEGRAMPEERDETAILS_H

#include "tqobject.h"

#include <QDateTime>
#include <QJSValue>

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

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(int participantsCount READ participantsCount NOTIFY participantsCountChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

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

    QString title() const;
    int participantsCount() const;
    QString statusText() const;

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

    void titleChanged();
    void participantsCountChanged();
    void statusTextChanged();

    void userFullChanged();
    void chatFullChanged();
    void chatUsersChanged();

public Q_SLOTS:

protected:
    void refresh();

    void connectChatSignals(class ChatObject *chat, bool dc = false);
    void connectUserSignals(class UserObject *user, bool dc = false);
    void connectDialogSignals(class DialogObject *dialog, bool dc = false);

    void insertChatFull(const class MessagesChatFull &result);

    QString convetDate(const QDateTime &td) const;

private:
    TelegramPeerDetailsPrivate *p;
};

#endif // TELEGRAMPEERDETAILS_H
