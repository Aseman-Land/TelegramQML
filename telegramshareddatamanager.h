#ifndef TELEGRAMSHAREDDATAMANAGER_H
#define TELEGRAMSHAREDDATAMANAGER_H

#include <QObject>
#include <QSharedPointer>

#include "telegramsharedpointer.h"
#include "telegramqml_global.h"

#include <telegram/objects/typeobjects.h>
#include <telegram/types/types.h>

class TelegramSharedDataManagerPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramSharedDataManager : public QObject
{
    Q_OBJECT
public:
    TelegramSharedDataManager(QObject *parent = 0);
    ~TelegramSharedDataManager();

    TelegramSharedPointer<DialogObject> insertDialog(const Dialog &dialog, QByteArray *key = 0);
    TelegramSharedPointer<MessageObject> insertMessage(const Message &message, QByteArray *key = 0x0);
    TelegramSharedPointer<ChatObject> insertChat(const Chat &chat, QByteArray *key = 0);
    TelegramSharedPointer<UserObject> insertUser(const User &user, QByteArray *key = 0);


    TelegramSharedPointer<DialogObject> getDialog(const QByteArray &byte);
    TelegramSharedPointer<MessageObject> getMessage(const QByteArray &byte);
    TelegramSharedPointer<ChatObject> getChat(const QByteArray &byte);
    TelegramSharedPointer<UserObject> getUser(const QByteArray &byte);

private:
    TelegramSharedDataManagerPrivate *p;
};

#endif // TELEGRAMSHAREDDATAMANAGER_H
