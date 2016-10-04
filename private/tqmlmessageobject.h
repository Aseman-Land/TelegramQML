#ifndef TQMLMESSAGEOBJECT_H
#define TQMLMESSAGEOBJECT_H

#include <telegram/objects/messageobject.h>
#include <telegram/objects/secretchatmessageobject.h>

class TQmlMessageObject : public MessageObject
{
    Q_OBJECT
    Q_PROPERTY(SecretChatMessageObject* secretChatMessage READ secretChatMessage WRITE setSecretChatMessage NOTIFY secretChatMessageChanged)

public:
    TQmlMessageObject(const Message &core, QObject *parent = 0) : MessageObject(core, parent), m_secretChatMessage(0) {}
    TQmlMessageObject(QObject *parent = 0) : MessageObject(parent), m_secretChatMessage(0) {}
    virtual ~TQmlMessageObject() { if(m_secretChatMessage) delete m_secretChatMessage; }

    SecretChatMessageObject *secretChatMessage() const { return m_secretChatMessage; }
    void setSecretChatMessage(SecretChatMessageObject *secretChatMessage) {
        if(m_secretChatMessage == secretChatMessage) return;
        if(m_secretChatMessage) delete m_secretChatMessage;
        m_secretChatMessage = secretChatMessage;
        if(m_secretChatMessage) {
            m_secretChatMessage->setParent(this);
        }
        Q_EMIT secretChatMessageChanged();
    }

    TQmlMessageObject &operator =(const Message &b) {
        MessageObject::operator =(b);
        return *this;
    }

    bool operator ==(const Message &b) const {
        return MessageObject::operator ==(b);
    }

Q_SIGNALS:
    void secretChatMessageChanged();

private:
    SecretChatMessageObject *m_secretChatMessage;
};

#endif // TQMLMESSAGEOBJECT_H
