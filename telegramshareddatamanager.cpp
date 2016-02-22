#define DECLARE_INSERT_FNC(TYPE, VARIABLE) \
    const QByteArray &_key = TelegramTools::identifier(VARIABLE); \
    if(key) *key = _key; \
    TYPE##Object *result = p->VARIABLE##s.value(_key); \
    if(result) { \
        *result = VARIABLE; \
        return result; \
    } \
    result = new TYPE##Object(VARIABLE); \
    connect(result, &TYPE##Object::destroyed, this, [this, _key](){ \
        p->VARIABLE##s.remove(_key); \
    }); \
    return result;

#define DECLARE_GET_FNC(VARIABLE) \
    return p->VARIABLE##s.value(byte);

#include "telegramshareddatamanager.h"
#include "telegramtools.h"

#include <QPointer>
#include <QByteArray>
#include <QHash>

class TelegramSharedDataManagerPrivate
{
public:
    QHash<QByteArray, DialogObject*> dialogs;
    QHash<QByteArray, MessageObject*> messages;
    QHash<QByteArray, ChatObject*> chats;
    QHash<QByteArray, UserObject*> users;
};

TelegramSharedDataManager::TelegramSharedDataManager(QObject *parent) :
    QObject(parent)
{
    p = new TelegramSharedDataManagerPrivate;
}

TelegramSharedPointer<DialogObject> TelegramSharedDataManager::insertDialog(const Dialog &dialog, QByteArray *key)
{
    DECLARE_INSERT_FNC(Dialog, dialog)
}

TelegramSharedPointer<MessageObject> TelegramSharedDataManager::insertMessage(const Message &message, QByteArray *key)
{
    DECLARE_INSERT_FNC(Message, message)
}

TelegramSharedPointer<ChatObject> TelegramSharedDataManager::insertChat(const Chat &chat, QByteArray *key)
{
    DECLARE_INSERT_FNC(Chat, chat)
}

TelegramSharedPointer<UserObject> TelegramSharedDataManager::insertUser(const User &user, QByteArray *key)
{
    DECLARE_INSERT_FNC(User, user)
}

TelegramSharedPointer<DialogObject> TelegramSharedDataManager::getDialog(const QByteArray &byte)
{
    DECLARE_GET_FNC(dialog)
}

TelegramSharedPointer<MessageObject> TelegramSharedDataManager::getMessage(const QByteArray &byte)
{
    DECLARE_GET_FNC(message)
}

TelegramSharedPointer<ChatObject> TelegramSharedDataManager::getChat(const QByteArray &byte)
{
    DECLARE_GET_FNC(chat)
}

TelegramSharedPointer<UserObject> TelegramSharedDataManager::getUser(const QByteArray &byte)
{
    DECLARE_GET_FNC(user)
}

TelegramSharedDataManager::~TelegramSharedDataManager()
{
    delete p;
}

