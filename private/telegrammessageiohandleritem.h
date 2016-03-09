#ifndef TELEGRAMMESSAGEIOHANDLERITEM_H
#define TELEGRAMMESSAGEIOHANDLERITEM_H

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "tqobject.h"

class ReplyMarkupObject;
class Message;
class MessageObject;
class TelegramEngine;
class InputPeerObject;
class TelegramMessageIOHandlerItemPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramMessageIOHandlerItem : public TqObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(int sendFileType READ sendFileType WRITE setSendFileType NOTIFY sendFileTypeChanged)
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(MessageObject* target READ target NOTIFY targetChanged)
    Q_PROPERTY(MessageObject* result READ result NOTIFY resultChanged)
    Q_PROPERTY(MessageObject* replyTo READ replyTo WRITE setReplyTo NOTIFY replyToChanged)
    Q_PROPERTY(ReplyMarkupObject* replyMarkup READ replyMarkup WRITE setReplyMarkup NOTIFY replyMarkupChanged)

public:

    enum Status {
        StatusNone,
        StatusDownloading,
        StatusUploading,
        StatusSending,
        StatusError,
        StatusDone
    };

    TelegramMessageIOHandlerItem(QObject *parent = 0);
    ~TelegramMessageIOHandlerItem();

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    void setCurrentPeer(InputPeerObject *peer);
    InputPeerObject *currentPeer() const;

    void setText(const QString &text);
    QString text() const;

    void setFile(const QString &file);
    QString file() const;

    void setSendFileType(int sendFileType);
    int sendFileType() const;

    void setReplyTo(MessageObject *message);
    MessageObject *replyTo() const;

    void setReplyMarkup(ReplyMarkupObject *markup);
    ReplyMarkupObject *replyMarkup() const;

    int status() const;

    void setTarget(MessageObject *object);
    MessageObject *target() const;

    MessageObject *result() const;
    void setResult(const Message &message);

    qint32 transfaredSize() const;
    qint32 totalSize() const;

    static QList<TelegramMessageIOHandlerItem*> getItems(TelegramEngine *engine, InputPeerObject *peer);
    QList<TelegramMessageIOHandlerItem*> getItems();

public Q_SLOTS:
    bool send();
    void download();
    void cancel();

Q_SIGNALS:
    void engineChanged();
    void currentPeerChanged();
    void textChanged();
    void fileChanged();
    void sendFileTypeChanged();
    void statusChanged();
    void targetChanged();
    void replyToChanged();
    void replyMarkupChanged();
    void resultChanged();
    void transfaredSizeChanged();
    void totalSizeChanged();
    void sent();

protected:
    bool sendMessage();
    bool sendFile();

    QByteArray identifier() const;
    Message createNewMessage();

    void setStatus(int status);
    void setTransfaredSize(qint32 size);
    void setTotalSize(qint32 size);

private:
    bool sendDocument();

private:
    TelegramMessageIOHandlerItemPrivate *p;
};

#endif // TELEGRAMMESSAGEIOHANDLERITEM_H
