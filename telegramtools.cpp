#include "telegramtools.h"

#include <QDataStream>
#include <telegram.h>

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
    case Peer::typePeerChannel:
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

Peer TelegramTools::chatPeer(const Chat &chat)
{
    Peer peer;
    switch(static_cast<int>(chat.classType()))
    {
    case Chat::typeChat:
        peer.setClassType(Peer::typePeerChat);
        peer.setChatId(chat.id());
        break;
    case Chat::typeChannel:
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
