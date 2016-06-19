#include "telegramchatsmemebrslistmodel.h"
#include "telegramsharedpointer.h"

#include <telegram/objects/typeobjects.h>
#include <telegram.h>

#include <QPointer>

class TelegramChatsMemebrsListModelPrivate
{
public:
    TelegramSharedPointer<InputPeerObject> inputPeer;
};

TelegramChatsMemebrsListModel::TelegramChatsMemebrsListModel(QObject *parent) :
    TelegramAbstractEngineListModel(parent)
{
    p = new TelegramChatsMemebrsListModelPrivate;
}

void TelegramChatsMemebrsListModel::setCurrentPeer(InputPeerObject *peer)
{
    if(p->inputPeer == peer)
        return;

    p->inputPeer = peer;
    refresh();
    Q_EMIT currentPeerChanged();
}

InputPeerObject *TelegramChatsMemebrsListModel::currentPeer() const
{

}

TelegramChatsMemebrsListModel::~TelegramChatsMemebrsListModel()
{
    delete p;
}

