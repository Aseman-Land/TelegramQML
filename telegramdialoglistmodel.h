#ifndef TELEGRAMDIALOGLISTMODEL_H
#define TELEGRAMDIALOGLISTMODEL_H

#include <QObject>
#include <QJSValue>

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractenginelistmodel.h"

class TelegramDialogListItem;
class TelegramDialogListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramDialogListModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_ENUMS(VisibilityFlags)
    Q_ENUMS(SortFlag)
    Q_PROPERTY(int visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QList<qint32> sortFlag READ sortFlag WRITE setSortFlag NOTIFY sortFlagChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)

public:
    enum VisibilityFlags {
        VisibilityEmptyDialogs = 1,
        VisibilityDeletedDialogs = 2,
        VisibilityOnlineUsersOnly = 4,
        VisibilityContacts = 8,
        VisibilityNonContacts = 16,
        VisibilityBots = 32,
        VisibilityUsers = VisibilityContacts | VisibilityNonContacts,
        VisibilityChats = 64,
        VisibilityChannels = 128,
        VisibilitySecretChats = 256,
        VisibilityAll = VisibilityEmptyDialogs | VisibilityDeletedDialogs | VisibilitySecretChats |
                        VisibilityUsers | VisibilityChats | VisibilityChannels | VisibilityBots
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
        RolePeerItem,

        RoleName,
        RoleMessageDate,
        RoleMessageUnread,
        RoleMessage,
        RoleLastOnline,
        RoleIsOnline,
        RoleStatus,
        RoleStatusText,
        RoleTyping,
        RoleUnreadCount,
        RoleMute
    };

    TelegramDialogListModel(QObject *parent = 0);
    ~TelegramDialogListModel();

    int visibility() const;
    void setVisibility(int visibility);

    QList<qint32> sortFlag() const;
    void setSortFlag(const QList<qint32> &sortFlag);

    QJSValue dateConvertorMethod() const;
    void setDateConvertorMethod(const QJSValue &method);

    bool refreshing() const;

    QByteArray id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

Q_SIGNALS:
    void visibilityChanged();
    void sortFlagChanged();
    void dateConvertorMethodChanged();
    void refreshingChanged();

public Q_SLOTS:

protected:
    void refresh();
    void clean();
    void resort();

    void timerEvent(QTimerEvent *e);

    virtual QString statusText(const TelegramDialogListItem &item) const;
    virtual QString convetDate(const QDateTime &td) const;

private:
    void getDialogsFromServer(const class InputPeer &offset, int limit, QHash<QByteArray, TelegramDialogListItem> *items = 0);
    class InputPeer processOnResult(const class MessagesDialogs &result, QHash<QByteArray, TelegramDialogListItem> *items);
    void changed(const QHash<QByteArray, TelegramDialogListItem> &items);
    QByteArrayList getSortedList(const QHash<QByteArray, TelegramDialogListItem> &items);

    void connectChatSignals(const QByteArray &id, class ChatObject *chat);
    void connectUserSignals(const QByteArray &id, class UserObject *user);
    void connectMessageSignals(const QByteArray &id, class MessageObject *message);
    void connectDialogSignals(const QByteArray &id, class DialogObject *dialog);

    virtual void onUpdateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShort(const Update &update, qint32 date);
    virtual void onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    virtual void onUpdates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

    void insertUpdate(const Update &update);
    void setRefreshing(bool stt);

private:
    TelegramDialogListModelPrivate *p;
};

#endif // TELEGRAMDIALOGLISTMODEL_H
