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
    Q_ENUMS(MessageType)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)
    Q_PROPERTY(QByteArray key READ key NOTIFY keyChanged)
    Q_PROPERTY(QVariantList typingUsers READ typingUsers NOTIFY typingUsersChanged)

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
        RoleDateTime,
        RoleDate,
        RoleUnread,
        RoleSent,
        RoleOut,
        RoleReplyMsgId,
        RoleReplyMessage,
        RoleReplyPeer,
        RoleForwardFromPeer,
        RoleForwardDate,
        RoleMessageType,

        RoleDownloadable,
        RoleDownloading,
        RoleDownloaded,
        RoleTransfaredSize,
        RoleTotalSize,
        RoleFilePath,
        RoleThumbPath
    };

    enum MessageType {
        TypeTextMessage,
        TypeDocumentMessage,
        TypeVideoMessage,
        TypeAudioMessage,
        TypeVenueMessage,
        TypeWebPageMessage,
        TypeGeoMessage,
        TypeContactMessage,
        TypeActionMessage,
        TypePhotoMessage,
        TypeStickerMessage,
        TypeAnimatedMessage,
        TypeUnsupportedMessage
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

    QByteArray key() const;
    QVariantList typingUsers() const;

Q_SIGNALS:
    void refreshingChanged();
    void currentPeerChanged();
    void keyChanged();
    void dateConvertorMethodChanged();
    void typingUsersChanged();

public Q_SLOTS:
    bool sendMessage(const QString &message, MessageObject *replyTo = 0, ReplyMarkupObject *replyMarkup = 0);
    bool sendFile(int type, const QString &file, MessageObject *replyTo = 0, ReplyMarkupObject *replyMarkup = 0);
    void markAsRead();

protected:
    void refresh();
    void clean();
    void resort();
    void setRefreshing(bool stt);
    QByteArray identifier() const;

    virtual QString convertDate(const QDateTime &td) const;
    MessageType messageType(MessageObject *msg) const;

private:
    void getMessagesFromServer(int offset, int limit, QHash<QByteArray, TelegramMessageListItem> *items = 0);
    void processOnResult(const class MessagesMessages &result, QHash<QByteArray, TelegramMessageListItem> *items);
    void changed(QHash<QByteArray, TelegramMessageListItem> hash);
    QByteArrayList getSortedList(const QHash<QByteArray, TelegramMessageListItem> &items);

    void connectMessageSignals(const QByteArray &id, class MessageObject *message);
    void connectChatSignals(const QByteArray &id, class ChatObject *chat);
    void connectUserSignals(const QByteArray &id, class UserObject *user);
    void connectHandlerSignals(const QByteArray &id, class TelegramMessageIOHandlerItem *handler);

    virtual void onUpdateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, const MessageFwdHeader &fwd_from, qint32 reply_to_msg_id, bool unread, bool out);
    virtual void onUpdateShort(const Update &update, qint32 date);
    virtual void onUpdatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    virtual void onUpdates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

    void insertUpdate(const Update &update);

private:
    TelegramMessageListModelPrivate *p;
};

#endif // TELEGRAMMESSAGELISTMODEL_H
