#ifndef TELEGRAMMESSAGELISTMODEL_H
#define TELEGRAMMESSAGELISTMODEL_H

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractlistmodel.h"

class TelegramMessageListItem;
class DialogObject;
class TelegramMessageListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramMessageListModel : public TelegramAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(DataRoles)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
    Q_PROPERTY(DialogObject* currentDialog READ currentDialog WRITE setCurrentDialog NOTIFY currentDialogChanged)

public:
    TelegramMessageListModel(QObject *parent = 0);
    ~TelegramMessageListModel();

    enum DataRoles {
        RoleMessageItem = Qt::UserRole,
        RoleMediaItem,
        RoleServiceItem,
        RoleMarkupItem,
        RoleEntityList,
        RoleFromUserItem,
        RoleToPeerItem,

        RoleMessage,
        RoleDate,
        RoleUnread,
        RoleSent,
        RoleOut,
        RoleReplyMessage,
        RoleForwardFromPeer,
        RoleForwardDate
    };

    bool refreshing() const;

    QByteArray id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    void setCurrentDialog(DialogObject *dialog);
    DialogObject *currentDialog() const;

Q_SIGNALS:
    void refreshingChanged();
    void currentDialogChanged();

public Q_SLOTS:

protected:
    void refresh();
    void clean();
    void setRefreshing(bool stt);

private:
    void getMessagesFromServer(int offset, int limit, QHash<QByteArray, TelegramMessageListItem> *items = 0);
    void processOnResult(const class MessagesMessages &result, QHash<QByteArray, TelegramMessageListItem> *items);
    void changed(const QHash<QByteArray, TelegramMessageListItem> &hash);

    void connectMessageSignals(const QByteArray &id, class MessageObject *message);
    void connectChatSignals(const QByteArray &id, class ChatObject *chat);
    void connectUserSignals(const QByteArray &id, class UserObject *user);

private:
    TelegramMessageListModelPrivate *p;
};

#endif // TELEGRAMMESSAGELISTMODEL_H
