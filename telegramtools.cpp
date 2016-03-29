#include "telegramtools.h"
#include "telegramsharedpointer.h"

#include <QDataStream>

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

QByteArray TelegramTools::identifier(const Peer &peer)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream << peer.classType();
    switch(static_cast<int>(peer.classType()))
    {
    case Peer::typePeerChannel:
        stream << peer.channelId();
        break;
    case Peer::typePeerChat:
        stream << peer.chatId();
        break;
    case Peer::typePeerUser:
        stream << peer.userId();
        break;
    }
    return res;
}

QByteArray TelegramTools::identifier(const InputPeer &peer)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream << peer.classType();
    switch(static_cast<int>(peer.classType()))
    {
    case InputPeer::typeInputPeerChannel:
        stream << peer.channelId();
        break;
    case InputPeer::typeInputPeerChat:
        stream << peer.chatId();
        break;
    case InputPeer::typeInputPeerUser:
        stream << peer.userId();
        break;
    }
    return res;
}

QByteArray TelegramTools::identifier(const Dialog &dialog)
{
    return identifier(dialog.peer());
}

QByteArray TelegramTools::identifier(const Dialog &dialog, qint32 messageId)
{
    return identifier(dialog.peer(), messageId);
}

QByteArray TelegramTools::identifier(const Peer &peer, qint32 messageId)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream << identifier(peer);
    stream << messageId;

    return res;
}

QByteArray TelegramTools::identifier(const Message &msg)
{
    return identifier(msg.toId(), msg.id());
}

QByteArray TelegramTools::identifier(const Chat &chat)
{
    return identifier(chatPeer(chat));
}

QByteArray TelegramTools::identifier(const User &user)
{
    return identifier(userPeer(user));
}

QByteArray TelegramTools::identifier(const UserFull &user)
{
    return identifier(user.user());
}

QByteArray TelegramTools::identifier(const ChatFull &chat)
{
    Peer peer(Peer::typePeerChat);
    peer.setChatId(chat.id());
    return identifier(peer);
}

InputPeer TelegramTools::chatInputPeer(const Chat &chat)
{
    InputPeer res;
    res.setAccessHash(chat.accessHash());
    switch(static_cast<int>(chat.classType()))
    {
    case Chat::typeChat:
        res.setClassType(InputPeer::typeInputPeerChat);
        res.setChatId(chat.id());
        break;
    case Chat::typeChannel:
        res.setClassType(InputPeer::typeInputPeerChannel);
        res.setChannelId(chat.id());
        break;
    }
    return res;
}

InputPeer TelegramTools::userInputPeer(const User &user)
{
    InputPeer res;
    res.setUserId(user.id());
    res.setAccessHash(user.accessHash());
    if(user.self())
        res.setClassType(InputPeer::typeInputPeerSelf);
    else
    if(user.deleted())
        res.setClassType(InputPeer::typeInputPeerEmpty);
    else
        res.setClassType(InputPeer::typeInputPeerUser);

    return res;
}

InputPeer TelegramTools::peerInputPeer(const Peer &peer)
{
    InputPeer res;
    switch(static_cast<uint>(peer.classType()))
    {
    case Peer::typePeerUser:
        res.setClassType(InputPeer::typeInputPeerUser);
        res.setUserId(peer.userId());
        break;
    case Peer::typePeerChat:
        res.setClassType(InputPeer::typeInputPeerChat);
        res.setChatId(peer.chatId());
        break;
    case Peer::typePeerChannel:
        res.setClassType(InputPeer::typeInputPeerChannel);
        res.setChannelId(peer.channelId());
        break;
    }
    return res;
}

Peer TelegramTools::chatPeer(const Chat &chat)
{
    Peer peer;
    switch(static_cast<int>(chat.classType()))
    {
    case Chat::typeChat:
    case Chat::typeChatForbidden:
        peer.setClassType(Peer::typePeerChat);
        peer.setChatId(chat.id());
        break;
    case Chat::typeChannel:
    case Chat::typeChannelForbidden:
        peer.setClassType(Peer::typePeerChannel);
        peer.setChannelId(chat.id());
        break;
    }
    return peer;
}

Peer TelegramTools::userPeer(const User &user)
{
    Peer peer;
    switch(static_cast<int>(user.classType()))
    {
    case User::typeUser:
        peer.setClassType(Peer::typePeerUser);
        peer.setUserId(user.id());
        break;
    }
    return peer;
}

Peer TelegramTools::messagePeer(const Message &msg)
{
    Peer peer = msg.toId();
    if(!msg.out() && peer.classType() == Peer::typePeerUser)
        peer.setUserId(msg.fromId());
    return peer;
}

Peer TelegramTools::inputPeerPeer(const InputPeer &inputPeer)
{
    Peer peer;
    switch(static_cast<uint>(inputPeer.classType()))
    {
    case InputPeer::typeInputPeerSelf:
    case InputPeer::typeInputPeerUser:
        peer.setClassType(Peer::typePeerUser);
        peer.setUserId(inputPeer.userId());
        break;
    case InputPeer::typeInputPeerChat:
        peer.setClassType(Peer::typePeerChat);
        peer.setChatId(inputPeer.chatId());
        break;
    case InputPeer::typeInputPeerChannel:
        peer.setClassType(Peer::typePeerChannel);
        peer.setChannelId(inputPeer.channelId());
        break;
    }
    return peer;
}

qint64 TelegramTools::generateRandomId()
{
    qint64 randomId;
    Utils::randomBytes(&randomId, 8);
    return randomId;
}

QString TelegramTools::convertErrorToText(const QString &error)
{
    if(error.isEmpty())
        return error;

    QString result = error.toLower();
    result.replace("_", " ");
    result[0] = result[0].toUpper();

    return result;
}

TelegramTypeQObject *TelegramTools::objectRoot(TelegramTypeQObject *object)
{
    TelegramTypeQObject *root = 0;
    TelegramTypeQObject *tmp = object;
    while(tmp)
    {
        root = tmp;
        tmp = qobject_cast<TelegramTypeQObject*>(tmp->parent());
    }
    return root;
}
