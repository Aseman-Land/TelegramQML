#ifndef TELEGRAMCHATSMEMEBRSLISTMODEL_H
#define TELEGRAMCHATSMEMEBRSLISTMODEL_H

#include "telegramabstractenginelistmodel.h"

class InputPeerObject;
class TelegramChatsMemebrsListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramChatsMemebrsListModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_PROPERTY(InputPeerObject currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)

public:
    TelegramChatsMemebrsListModel(QObject *parent = 0);
    ~TelegramChatsMemebrsListModel();

    void setCurrentPeer(InputPeerObject *peer);
    InputPeerObject *currentPeer() const;

Q_SIGNALS:
    void currentPeerChanged();

public Q_SLOTS:

private:
    TelegramChatsMemebrsListModelPrivate *p;
};

#endif // TELEGRAMCHATSMEMEBRSLISTMODEL_H
