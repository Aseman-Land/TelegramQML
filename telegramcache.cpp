#define DECLARE_INSERT(VALUE, FOLDER_PATH) \
    QDir().mkpath(FOLDER_PATH); \
    const QByteArray &key = TelegramTools::identifier(VALUE); \
    writeMap(QString(FOLDER_PATH + "/" + key.toHex()), VALUE.toMap());

#include "telegramcache.h"
#include "telegramtools.h"

#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QDir>
#include <QQmlEngine>
#include <QtQml>

class TelegramCachePrivate
{
public:
    QString path;
    QJSValue encryptMethod;
    QJSValue decryptMethod;
};

class SortUnitType
{
public:
    SortUnitType(): num(0){}
    QChar ch;
    quint64 num;
};

QList<SortUnitType> analize_file_name(const QString &fileName)
{
    QList<SortUnitType> res;
    for(int i=0; i<fileName.length(); i++)
    {
        const QChar &ch = fileName[i];
        if(ch.isNumber())
        {
            int num = QString(ch).toInt();
            if(res.isEmpty() || !res.last().ch.isNull() )
                res << SortUnitType();

            SortUnitType & resUnit = res[res.length()-1];
            resUnit.num = resUnit.num*10 + num;
        }
        else
        {
            SortUnitType unit;
            unit.ch = ch;
            res << unit;
        }
    }

    return res;
}

bool fileListSort(const QFileInfo &f1, const QFileInfo &f2)
{
    const QString & s1 = f1.fileName();
    const QString & s2 = f2.fileName();

    const QList<SortUnitType> &ul1 = analize_file_name(s1);
    const QList<SortUnitType> &ul2 = analize_file_name(s2);

    for(int i=0; i<ul1.length() && i<ul2.length(); i++)
    {
        const SortUnitType &u1 = ul1.at(i);
        const SortUnitType &u2 = ul2.at(i);

        if(u1.ch.isNull() && !u2.ch.isNull())
            return true;
        if(!u1.ch.isNull() && u2.ch.isNull())
            return false;
        if(!u1.ch.isNull() && !u2.ch.isNull())
        {
            if(u1.ch < u2.ch)
                return true;
            if(u1.ch > u2.ch)
                return false;
        }
        if(u1.ch.isNull() && u2.ch.isNull())
        {
            if(u1.num < u2.num)
                return true;
            if(u1.num > u2.num)
                return false;
        }
    }

    return ul1.length() < ul2.length();
}

TelegramCache::TelegramCache(QObject *parent) :
    QObject(parent)
{
    p = new TelegramCachePrivate;
}

void TelegramCache::setPath(const QString &path)
{
    if(p->path == path)
        return;

    p->path = path;
    Q_EMIT pathChanged();
}

QString TelegramCache::path() const
{
    return p->path;
}

void TelegramCache::setEncryptMethod(const QJSValue &method)
{
    if(p->encryptMethod.isNull() && method.isNull())
        return;

    p->encryptMethod = method;
    Q_EMIT encryptMethodChanged();
}

QJSValue TelegramCache::encryptMethod() const
{
    return p->encryptMethod;
}

void TelegramCache::setDecryptMethod(const QJSValue &method)
{
    if(p->decryptMethod.isNull() && method.isNull())
        return;

    p->decryptMethod = method;
    Q_EMIT decryptMethodChanged();
}

QJSValue TelegramCache::decryptMethod() const
{
    return p->decryptMethod;
}

void TelegramCache::insert(const User &user)
{
    const QString folderPath = p->path + "/users";
    DECLARE_INSERT(user, folderPath);
}

void TelegramCache::insert(const Chat &chat)
{
    const QString folderPath = p->path + "/chats";
    DECLARE_INSERT(chat, folderPath);
}

void TelegramCache::insert(const ChatFull &chat)
{
    const QString folderPath = p->path + "/chatfulls";
    DECLARE_INSERT(chat, folderPath);
}

void TelegramCache::insert(const UserFull &user)
{
    const QString folderPath = p->path + "/userfulls";
    DECLARE_INSERT(user, folderPath);
}

void TelegramCache::insert(const QList<Dialog> &dialogs)
{
    const QString filePath = p->path + "/dialogs";

    QVariantList list;
    Q_FOREACH(const Dialog &dlg, dialogs)
        list << dlg.toMap();

    writeList(filePath, list);
}

void TelegramCache::insert(const Message &msg)
{
    const QString folderPath = getMessageFolder(TelegramTools::messagePeer(msg));
    const QString filePath = folderPath + "/" + QString::number(msg.id());
    writeMap(filePath, msg.toMap());
}

MessagesMessages TelegramCache::readMessages(const InputPeer &peer, int offset, int limit) const
{
    return readMessages(TelegramTools::inputPeerPeer(peer), offset, limit);
}

MessagesMessages TelegramCache::readMessages(const Peer &peer, int offset, int limit) const
{
    MessagesMessages result(MessagesMessages::typeMessagesMessages);

    const QString folderPath = getMessageFolder(peer);
    QStringList files = QDir(folderPath).entryList(QDir::Files);
    qStableSort(files.begin(), files.end(), fileListSort);

    files = files.mid(offset, limit);

    QHash<QByteArray, Chat> chats;
    QHash<QByteArray, User> users;

    QList<Message> messages;
    Q_FOREACH(const QString &f, files)
    {
        const QString path = folderPath + "/" + f;
        const QMap<QString, QVariant> &map = readMap(path);
        if(map.isEmpty())
            continue;

        const Message &msg = Message::fromMap(map);

        const Peer &toPeer = msg.toId();
        const QByteArray &toKey = TelegramTools::identifier(toPeer);
        switch(static_cast<int>(toPeer.classType()))
        {
        case Peer::typePeerChannel:
        case Peer::typePeerChat:
            if(!chats.contains(toKey))
                chats[toKey] = readChat(toPeer);
            break;
        case Peer::typePeerUser:
            if(!users.contains(toKey))
                users[toKey] = readUser(toPeer);
            break;
        }

        if(msg.fromId())
        {
            Peer fromPeer(Peer::typePeerUser);
            fromPeer.setUserId(msg.fromId());
            const QByteArray &fromKey = TelegramTools::identifier(fromPeer);
            switch(static_cast<int>(fromPeer.classType()))
            {
            case Peer::typePeerChannel:
            case Peer::typePeerChat:
                if(!chats.contains(fromKey))
                    chats[fromKey] = readChat(fromPeer);
                break;
            case Peer::typePeerUser:
                if(!users.contains(fromKey))
                    users[fromKey] = readUser(fromPeer);
                break;
            }
        }

        messages << msg;
    }

    result.setMessages(messages);
    result.setChats(chats.values());
    result.setUsers(users.values());
    result.setCount(messages.count());

    return result;
}

Chat TelegramCache::readChat(const InputPeer &peer) const
{
    return readChat(TelegramTools::inputPeerPeer(peer));
}

Chat TelegramCache::readChat(const Peer &peer) const
{
    const QString folderPath = p->path + "/chats";
    const QString filePath = folderPath + "/" + TelegramTools::identifier(peer).toHex();
    const QMap<QString, QVariant> &map = readMap(filePath);
    return Chat::fromMap(map);
}

ChatFull TelegramCache::readChatFull(const InputPeer &peer) const
{
    return readChatFull(TelegramTools::inputPeerPeer(peer));
}

ChatFull TelegramCache::readChatFull(const Peer &peer) const
{
    const QString folderPath = p->path + "/chatfulls";
    const QString filePath = folderPath + "/" + TelegramTools::identifier(peer).toHex();
    const QMap<QString, QVariant> &map = readMap(filePath);
    return ChatFull::fromMap(map);
}

User TelegramCache::readUser(const InputPeer &peer) const
{
    return readUser(TelegramTools::inputPeerPeer(peer));
}

User TelegramCache::readUser(const Peer &peer) const
{
    const QString folderPath = p->path + "/users";
    const QString filePath = folderPath + "/" + TelegramTools::identifier(peer).toHex();
    const QMap<QString, QVariant> &map = readMap(filePath);
    return User::fromMap(map);
}

UserFull TelegramCache::readUserFull(const InputPeer &peer) const
{
    return readUserFull(TelegramTools::inputPeerPeer(peer));
}

UserFull TelegramCache::readUserFull(const Peer &peer) const
{
    const QString folderPath = p->path + "/userfulls";
    const QString filePath = folderPath + "/" + TelegramTools::identifier(peer).toHex();
    const QMap<QString, QVariant> &map = readMap(filePath);
    return UserFull::fromMap(map);
}

MessagesDialogs TelegramCache::readDialogs() const
{
    MessagesDialogs result(MessagesDialogs::typeMessagesDialogs);

    const QString filePath = p->path + "/dialogs";
    const QList<QVariant> &list = readList(filePath);
    QList<Dialog> dialogs;

    QHash<QByteArray, Chat> chats;
    QHash<QByteArray, User> users;
    QHash<QByteArray, Message> messages;

    Q_FOREACH(const QVariant &var, list)
    {
        const Dialog &dialog = Dialog::fromMap( var.toMap() );

        const Peer &peer = dialog.peer();
        const QByteArray &key = TelegramTools::identifier(peer);
        switch(static_cast<int>(peer.classType()))
        {
        case Peer::typePeerChannel:
        case Peer::typePeerChat:
            if(!chats.contains(key))
                chats[key] = readChat(peer);
            break;
        case Peer::typePeerUser:
            if(!users.contains(key))
                users[key] = readUser(peer);
            break;
        }

        if(dialog.topMessage())
        {
            QByteArray topMsgKey = TelegramTools::identifier(dialog.peer(), dialog.topMessage());
            const QString &messageFolder = getMessageFolder(peer);
            const QString messageFile = messageFolder + "/" + QString::number(dialog.topMessage());
            messages[topMsgKey] = Message::fromMap( readMap(messageFile) );
        }

        dialogs << dialog;
    }

    result.setDialogs(dialogs);
    result.setChats(chats.values());
    result.setUsers(users.values());
    result.setMessages(messages.values());
    result.setCount(dialogs.count());

    return result;
}

QStringList TelegramCache::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(path);
}

QString TelegramCache::getMessageFolder(const Peer &peer) const
{
    const QByteArray &peerKey = TelegramTools::identifier(peer);
    const QString folderPath = p->path + "/messages/" + peerKey.toHex();
    QDir().mkpath(folderPath);
    return folderPath;
}

QMap<QString, QVariant> TelegramCache::readMap(const QString &path) const
{
    QMap<QString, QVariant> result;
    QByteArray data = read(path);
    if(data.isEmpty())
        return result;

    QDataStream stream(&data, QIODevice::WriteOnly);
    stream >> result;
    return result;
}

bool TelegramCache::writeMap(const QString &path, const QMap<QString, QVariant> &map) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << map;

    if(p->encryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(this);
        if(engine) {
            QJSValue res = p->encryptMethod.call(QJSValueList()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    return write(path, data);
}

QList<QVariant> TelegramCache::readList(const QString &path) const
{
    QList<QVariant> result;
    QByteArray data = read(path);
    if(data.isEmpty())
        return result;

    QDataStream stream(&data, QIODevice::WriteOnly);
    stream >> result;
    return result;
}

bool TelegramCache::writeList(const QString &path, const QList<QVariant> &list) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << list;

    if(p->encryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(this);
        if(engine) {
            QJSValue res = p->encryptMethod.call(QJSValueList()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    return write(path, data);
}

QByteArray TelegramCache::read(const QString &path) const
{
    QByteArray data;
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
        return data;

    data = file.readAll();
    file.close();

    if(p->decryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(this);
        if(engine) {
            QJSValue res = p->decryptMethod.call(QJSValueList()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    return data;
}

bool TelegramCache::write(const QString &path, QByteArray data) const
{
    QFile file(path);
    if(!file.open(QFile::WriteOnly))
        return false;

    if(p->encryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(this);
        if(engine) {
            QJSValue res = p->encryptMethod.call(QJSValueList()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    file.write(data);
    file.close();
    return true;
}

TelegramCache::~TelegramCache()
{
    delete p;
}

