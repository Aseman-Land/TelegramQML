#include "telegramabstractenginelistmodel.h"

#include <telegram.h>
#include <QHash>

TelegramAbstractEngineListModel::TelegramAbstractEngineListModel(QObject *parent) :
    TelegramAbstractListModel(parent),
    mTelegram(0)
{
}

void TelegramAbstractEngineListModel::setEngine(TelegramEngine *engine) {
    if(mEngine == engine)
        return;
    if(mEngine)
    {
        disconnect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractEngineListModel::connectTelegram);
        disconnect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractEngineListModel::refresh);
        disconnect(mEngine.data(), &TelegramEngine::stateChanged, this, &TelegramAbstractEngineListModel::refresh);
    }

    mEngine = engine;
    if(mEngine)
    {
        connect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractEngineListModel::connectTelegram);
        connect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractEngineListModel::refresh);
        connect(mEngine.data(), &TelegramEngine::stateChanged, this, &TelegramAbstractEngineListModel::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

void TelegramAbstractEngineListModel::onUpdatesGetDifferenceAnswer(qint64, const QList<Message> &, const QList<SecretChatMessage> &, const QList<Update> &, const QList<Chat> &, const QList<User> &, const UpdatesState &, bool )
{
}

void TelegramAbstractEngineListModel::onUpdatesGetStateAnswer(qint64, const UpdatesState &)
{

}

void TelegramAbstractEngineListModel::onUpdatesGetChannelDifferenceAnswer(qint64, const UpdatesChannelDifference &)
{

}

void TelegramAbstractEngineListModel::onUpdateSecretChatMessage(const SecretChatMessage &, qint32)
{
}

void TelegramAbstractEngineListModel::onUpdatesTooLong()
{
}

void TelegramAbstractEngineListModel::onUpdateShortMessage(qint32, qint32, const QString &, qint32, qint32, qint32, const MessageFwdHeader &fwd_from, qint32, bool, bool)
{
}

void TelegramAbstractEngineListModel::onUpdateShortChatMessage(qint32, qint32, qint32, const QString &, qint32, qint32, qint32, const MessageFwdHeader &fwd_from, qint32, bool, bool)
{
}

void TelegramAbstractEngineListModel::onUpdateShort(const Update &, qint32)
{
}

void TelegramAbstractEngineListModel::onUpdatesCombined(const QList<Update> &, const QList<User> &, const QList<Chat> &, qint32, qint32, qint32)
{
}

void TelegramAbstractEngineListModel::onUpdates(const QList<Update> &, const QList<User> &, const QList<Chat> &, qint32, qint32 )
{
}

void TelegramAbstractEngineListModel::startTimer(int ms, TelegramAbstractEngineListModel::Callback callback)
{
    if(ms <= 0)
    {
        callback();
        return;
    }

    const int id = QObject::startTimer(ms);
    mTimers[id] = callback;
}

void TelegramAbstractEngineListModel::timerEvent(QTimerEvent *e)
{
    const int timerId = e->timerId();
    if(mTimers.contains(timerId))
    {
        Callback callback = mTimers.take(timerId);
        killTimer(timerId);
        callback();
    }

    QAbstractItemModel::timerEvent(e);
}

void TelegramAbstractEngineListModel::connectTelegram()
{
    if(mTelegram == mEngine->telegram())
        return;

    if(mTelegram)
    {
        disconnect(mTelegram.data(), &Telegram::updates, this, &TelegramAbstractEngineListModel::onUpdates);
        disconnect(mTelegram.data(), &Telegram::updatesCombined, this, &TelegramAbstractEngineListModel::onUpdatesCombined);
        disconnect(mTelegram.data(), &Telegram::updatesTooLong, this, &TelegramAbstractEngineListModel::onUpdatesTooLong);
        disconnect(mTelegram.data(), &Telegram::updateSecretChatMessage, this, &TelegramAbstractEngineListModel::onUpdateSecretChatMessage);
        disconnect(mTelegram.data(), &Telegram::updateShort, this, &TelegramAbstractEngineListModel::onUpdateShort);
        disconnect(mTelegram.data(), &Telegram::updateShortChatMessage, this, &TelegramAbstractEngineListModel::onUpdateShortChatMessage);
        disconnect(mTelegram.data(), &Telegram::updateShortMessage, this, &TelegramAbstractEngineListModel::onUpdateShortMessage);
        disconnect(mTelegram.data(), &Telegram::updatesGetDifferenceAnswer, this, &TelegramAbstractEngineListModel::onUpdatesGetDifferenceAnswer);
        disconnect(mTelegram.data(), &Telegram::updatesGetStateAnswer, this, &TelegramAbstractEngineListModel::onUpdatesGetStateAnswer);
        disconnect(mTelegram.data(), &Telegram::updatesGetChannelDifferenceAnswer, this, &TelegramAbstractEngineListModel::onUpdatesGetChannelDifferenceAnswer);
    }

    mTelegram = mEngine->telegram();
    if(mTelegram)
    {
        connect(mTelegram.data(), &Telegram::updates, this, &TelegramAbstractEngineListModel::onUpdates);
        connect(mTelegram.data(), &Telegram::updatesCombined, this, &TelegramAbstractEngineListModel::onUpdatesCombined);
        connect(mTelegram.data(), &Telegram::updatesTooLong, this, &TelegramAbstractEngineListModel::onUpdatesTooLong);
        connect(mTelegram.data(), &Telegram::updateSecretChatMessage, this, &TelegramAbstractEngineListModel::onUpdateSecretChatMessage);
        connect(mTelegram.data(), &Telegram::updateShort, this, &TelegramAbstractEngineListModel::onUpdateShort);
        connect(mTelegram.data(), &Telegram::updateShortChatMessage, this, &TelegramAbstractEngineListModel::onUpdateShortChatMessage);
        connect(mTelegram.data(), &Telegram::updateShortMessage, this, &TelegramAbstractEngineListModel::onUpdateShortMessage);
        connect(mTelegram.data(), &Telegram::updatesGetDifferenceAnswer, this, &TelegramAbstractEngineListModel::onUpdatesGetDifferenceAnswer);
        connect(mTelegram.data(), &Telegram::updatesGetStateAnswer, this, &TelegramAbstractEngineListModel::onUpdatesGetStateAnswer);
        connect(mTelegram.data(), &Telegram::updatesGetChannelDifferenceAnswer, this, &TelegramAbstractEngineListModel::onUpdatesGetChannelDifferenceAnswer);
    }
}

TelegramAbstractEngineListModel::~TelegramAbstractEngineListModel()
{
}

