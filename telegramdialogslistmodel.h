#ifndef TELEGRAMDIALOGSLISTMODEL_H
#define TELEGRAMDIALOGSLISTMODEL_H

#include <QObject>
#include <QJSValue>

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractlistmodel.h"

class TelegramDialogsListItem;
class TelegramDialogsListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramDialogsListModel : public TelegramAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(VisibilityFlags)
    Q_ENUMS(SortFlag)
    Q_PROPERTY(int visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QList<qint32> sortFlag READ sortFlag WRITE setSortFlag NOTIFY sortFlagChanged)
    Q_PROPERTY(QJSValue dateConversation READ dateConversation WRITE setDateConversation NOTIFY dateConversationChanged)

public:
    enum VisibilityFlags {
        VisibilityEmptyDialogs = 1,
        VisibilityDeletedDialogs = 2,
        VisibilityOnlineUsersOnly = 4,
        VisibilityContacts = 8,
        VisibilityNonContacts = 16,
        VisibilityUsers = VisibilityContacts | VisibilityNonContacts,
        VisibilityChats = 32,
        VisibilityChannels = 64,
        VisibilitySecretChats = 128,
        VisibilityAll = VisibilityEmptyDialogs | VisibilityDeletedDialogs | VisibilityOnlineUsersOnly |
                        VisibilityUsers | VisibilityChats | VisibilityChannels | VisibilitySecretChats
    };

    enum SortFlag {
        SortByType = 0,
        SortByName = 1,
        SortByDate = 2,
        SortByUnreads = 3,
        SortByOnline = 4
    };

    enum DataRoles {
        RoleDialogItem = Qt::UserRole,
        RoleChatItem,
        RoleUserItem,
        RoleTopMessageItem,

        RoleName,
        RoleMessageDate,
        RoleMessageUnread,
        RoleMessage,
        RoleLastOnline,
        RoleIsOnline,
        RoleStatus,
        RoleStatusText,
        RoleTyping,
        RoleUnreadCount
    };

    TelegramDialogsListModel(QObject *parent = 0);
    ~TelegramDialogsListModel();

    int visibility() const;
    void setVisibility(int visibility);

    QList<qint32> sortFlag() const;
    void setSortFlag(const QList<qint32> &sortFlag);

    QJSValue dateConversation() const;
    void setDateConversation(const QJSValue &method);

    QByteArray id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

Q_SIGNALS:
    void visibilityChanged();
    void sortFlagChanged();
    void dateConversationChanged();

public Q_SLOTS:

protected:
    void refresh();
    void clean();
    void resort();

    void timerEvent(QTimerEvent *e);

    virtual QString statusText(const TelegramDialogsListItem &item) const;
    virtual QString convetDate(const QDateTime &td) const;

private:
    void getDialogsFromServer(const class InputPeer &offset, int limit, QHash<QByteArray, TelegramDialogsListItem> *items = 0);
    class InputPeer processOnResult(const class MessagesDialogs &result, QHash<QByteArray, TelegramDialogsListItem> *items);
    void changed(const QHash<QByteArray, TelegramDialogsListItem> &items);
    QByteArrayList getSortedList(const QHash<QByteArray, TelegramDialogsListItem> &items);

    void connectChatSignals(const QByteArray &id, class ChatObject *chat);
    void connectUserSignals(const QByteArray &id, class UserObject *user);
    void connectMessageSignals(const QByteArray &id, class MessageObject *message);
    void connectDialogSignals(const QByteArray &id, class DialogObject *dialog);

    virtual void onUpdateShort(const Update &update, qint32 date);
    virtual void onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    virtual void onUpdates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

    void insertUpdate(const Update &update);

private:
    TelegramDialogsListModelPrivate *p;
};

#endif // TELEGRAMDIALOGSLISTMODEL_H
