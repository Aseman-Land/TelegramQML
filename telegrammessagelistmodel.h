#ifndef TELEGRAMMESSAGELISTMODEL_H
#define TELEGRAMMESSAGELISTMODEL_H

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractenginelistmodel.h"

#include <QJSValue>

class ReplyMarkupObject;
class MessageObject;
class TelegramMessageListItem;
class InputPeerObject;
class TelegramMessageListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramMessageListModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_ENUMS(DataRoles)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)

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
        RoleIsSticker,
        RoleIsAnimated,
        RoleReplyMsgId,
        RoleReplyMessage,
        RoleReplyPeer,
        RoleForwardFromPeer,
        RoleForwardDate,

        RoleDownloadable,
        RoleDownloading,
        RoleDownloaded,
        RoleTransfaredSize,
        RoleTotalSize,
        RoleFilePath,
        RoleThumbPath
    };

    bool refreshing() const;

    QByteArray id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    void setCurrentPeer(InputPeerObject *dialog);
    InputPeerObject *currentPeer() const;

    QJSValue dateConvertorMethod() const;
    void setDateConvertorMethod(const QJSValue &method);

Q_SIGNALS:
    void refreshingChanged();
    void currentPeerChanged();
    void dateConvertorMethodChanged();

public Q_SLOTS:
    bool sendMessage(const QString &message, MessageObject *replyTo = 0, ReplyMarkupObject *replyMarkup = 0);
    bool sendFile(int type, const QString &file, MessageObject *replyTo = 0, ReplyMarkupObject *replyMarkup = 0);

protected:
    void refresh();
    void clean();
    void resort();
    void setRefreshing(bool stt);
    QByteArray identifier() const;

    virtual QString convertDate(const QDateTime &td) const;

private:
    void getMessagesFromServer(int offset, int limit, QHash<QByteArray, TelegramMessageListItem> *items = 0);
    void processOnResult(const class MessagesMessages &result, QHash<QByteArray, TelegramMessageListItem> *items);
    void changed(QHash<QByteArray, TelegramMessageListItem> hash);
    QByteArrayList getSortedList(const QHash<QByteArray, TelegramMessageListItem> &items);

    void connectMessageSignals(const QByteArray &id, class MessageObject *message);
    void connectChatSignals(const QByteArray &id, class ChatObject *chat);
    void connectUserSignals(const QByteArray &id, class UserObject *user);
    void connectHandlerSignals(const QByteArray &id, class TelegramMessageIOHandlerItem *handler);

private:
    TelegramMessageListModelPrivate *p;
};

#endif // TELEGRAMMESSAGELISTMODEL_H
