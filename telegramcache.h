#ifndef TELEGRAMCACHE_H
#define TELEGRAMCACHE_H

#include <QJSValue>
#include <QObject>

#include <telegram/types/types.h>

class TelegramCachePrivate;
class TelegramCache : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QJSValue encryptMethod READ encryptMethod WRITE setEncryptMethod NOTIFY encryptMethodChanged)
    Q_PROPERTY(QJSValue decryptMethod READ decryptMethod WRITE setDecryptMethod NOTIFY decryptMethodChanged)

public:
    TelegramCache(QObject *parent = 0);
    ~TelegramCache();

    void setPath(const QString &path);
    QString path() const;

    void setEncryptMethod(const QJSValue &method);
    QJSValue encryptMethod() const;

    void setDecryptMethod(const QJSValue &method);
    QJSValue decryptMethod() const;

    void insert(const Message &message);
    void insert(const User &user);
    void insert(const Chat &chat);
    void insert(const ChatFull &chat);
    void insert(const UserFull &user);
    void insert(const QList<Dialog> &dialogs);

    MessagesMessages readMessages(const InputPeer &peer, int offset, int limit) const;
    MessagesMessages readMessages(const Peer &peer, int offset, int limit) const;

    Chat readChat(const InputPeer &peer) const;
    Chat readChat(const Peer &peer) const;

    ChatFull readChatFull(const InputPeer &peer) const;
    ChatFull readChatFull(const Peer &peer) const;

    User readUser(const InputPeer &peer) const;
    User readUser(const Peer &peer) const;

    UserFull readUserFull(const InputPeer &peer) const;
    UserFull readUserFull(const Peer &peer) const;

    MessagesDialogs readDialogs() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void pathChanged();
    void encryptMethodChanged();
    void decryptMethodChanged();

protected:
    QString getMessageFolder(const Peer &peer) const;

    QMap<QString, QVariant> readMap(const QString &path) const;
    bool writeMap(const QString &path, const QMap<QString, QVariant> &map) const;

    QList<QVariant> readList(const QString &path) const;
    bool writeList(const QString &path, const QList<QVariant> &list) const;

    QByteArray read(const QString &path) const;
    bool write(const QString &path, QByteArray data) const;

private:
    TelegramCachePrivate *p;
};

#endif // TELEGRAMCACHE_H
