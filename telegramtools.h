#ifndef TELEGRAMTOOLS_H
#define TELEGRAMTOOLS_H

#include "telegramqml_global.h"
#include <QByteArray>

class TelegramTypeQObject;
class TELEGRAMQMLSHARED_EXPORT TelegramTools
{
public:
    static QByteArray identifier(const class Peer &peer);
    static QByteArray identifier(const class InputPeer &peer);
    static QByteArray identifier(const class Dialog &dialog);
    static QByteArray identifier(const class Dialog &dialog, qint32 messageId);
    static QByteArray identifier(const class Peer &peer, qint32 messageId);
    static QByteArray identifier(const class Message &message);
    static QByteArray identifier(const class Chat &chat);
    static QByteArray identifier(const class User &user);
    static QByteArray identifier(const class UserFull &user);
    static QByteArray identifier(const class ChatFull &chat);

    static class InputPeer chatInputPeer(const class Chat &chat);
    static class InputPeer userInputPeer(const class User &user);
    static class InputPeer peerInputPeer(const class Peer &peer, qint64 accessHash);

    static class Peer chatPeer(const class Chat &chat);
    static class Peer userPeer(const class User &user);
    static class Peer messagePeer(const class Message &message);
    static class Peer inputPeerPeer(const class InputPeer &inputPeer);

    static qint64 generateRandomId();
    static QString convertErrorToText(const QString &error);

    static TelegramTypeQObject *objectRoot(TelegramTypeQObject *object);
};

#endif // TELEGRAMTOOLS_H
