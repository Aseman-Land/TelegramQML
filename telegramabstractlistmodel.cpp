#include "telegramabstractlistmodel.h"

#include <telegram.h>
#include <QHash>

TelegramAbstractListModel::TelegramAbstractListModel(QObject *parent) :
    QAbstractListModel(parent),
    mTelegram(0)
{
}

QStringList TelegramAbstractListModel::roles() const
{
    QStringList result;
    const QHash<int,QByteArray> &roles = roleNames();
    QHashIterator<int,QByteArray> i(roles);
    while(i.hasNext())
    {
        i.next();
        result << i.value();
    }

    qSort(result.begin(), result.end());
    return result;
}

void TelegramAbstractListModel::setEngine(TelegramEngine *engine) {
    if(mEngine == engine)
        return;
    if(mEngine)
    {
        disconnect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractListModel::connectTelegram);
        disconnect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractListModel::refresh);
        disconnect(mEngine.data(), &TelegramEngine::stateChanged, this, &TelegramAbstractListModel::refresh);
    }

    mEngine = engine;
    if(mEngine)
    {
        connect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractListModel::connectTelegram);
        connect(mEngine.data(), &TelegramEngine::telegramChanged, this, &TelegramAbstractListModel::refresh);
        connect(mEngine.data(), &TelegramEngine::stateChanged, this, &TelegramAbstractListModel::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

QVariant TelegramAbstractListModel::get(int row, int role) const
{
    if(row >= count() || row < 0)
        return QVariant();

    const QModelIndex &idx = index(row,0);
    return data(idx , role);
}

QVariant TelegramAbstractListModel::get(int index, const QString &roleName) const
{
    const int role = roleNames().key(roleName.toUtf8());
    return get(index, role);
}

QVariantMap TelegramAbstractListModel::get(int index) const
{
    if(index >= count())
        return QVariantMap();

    QVariantMap result;
    const QHash<int,QByteArray> &roles = roleNames();
    QHashIterator<int,QByteArray> i(roles);
    while(i.hasNext())
    {
        i.next();
        result[i.value()] = get(index, i.key());
    }

    return result;
}

void TelegramAbstractListModel::onUpdatesGetDifferenceAnswer(qint64, const QList<Message> &, const QList<SecretChatMessage> &, const QList<Update> &, const QList<Chat> &, const QList<User> &, const UpdatesState &, bool )
{
}

void TelegramAbstractListModel::onUpdatesGetStateAnswer(qint64, const UpdatesState &)
{

}

void TelegramAbstractListModel::onUpdatesGetChannelDifferenceAnswer(qint64, const UpdatesChannelDifference &)
{

}

void TelegramAbstractListModel::onUpdateSecretChatMessage(const SecretChatMessage &, qint32)
{
}

void TelegramAbstractListModel::onUpdatesTooLong()
{
}

void TelegramAbstractListModel::onUpdateShortMessage(qint32, qint32, const QString &, qint32, qint32, qint32, Peer, qint32, qint32, bool, bool)
{
}

void TelegramAbstractListModel::onUpdateShortChatMessage(qint32, qint32, qint32, const QString &, qint32, qint32, qint32, Peer, qint32, qint32, bool, bool)
{
}

void TelegramAbstractListModel::onUpdateShort(const Update &, qint32)
{
}

void TelegramAbstractListModel::onUpdatesCombined(const QList<Update> &, const QList<User> &, const QList<Chat> &, qint32, qint32, qint32)
{
}

void TelegramAbstractListModel::onUpdates(const QList<Update> &, const QList<User> &, const QList<Chat> &, qint32, qint32 )
{
}

void TelegramAbstractListModel::startTimer(int ms, TelegramAbstractListModel::Callback callback)
{
    if(ms <= 0)
    {
        callback();
        return;
    }

    const int id = QObject::startTimer(ms);
    mTimers[id] = callback;
}

void TelegramAbstractListModel::timerEvent(QTimerEvent *e)
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

void TelegramAbstractListModel::connectTelegram()
{
    if(mTelegram == mEngine->telegram())
        return;

    if(mTelegram)
    {
        disconnect(mTelegram.data(), &Telegram::updates, this, &TelegramAbstractListModel::onUpdates);
        disconnect(mTelegram.data(), &Telegram::updatesCombined, this, &TelegramAbstractListModel::onUpdatesCombined);
        disconnect(mTelegram.data(), &Telegram::updatesTooLong, this, &TelegramAbstractListModel::onUpdatesTooLong);
        disconnect(mTelegram.data(), &Telegram::updateSecretChatMessage, this, &TelegramAbstractListModel::onUpdateSecretChatMessage);
        disconnect(mTelegram.data(), &Telegram::updateShort, this, &TelegramAbstractListModel::onUpdateShort);
        disconnect(mTelegram.data(), &Telegram::updateShortChatMessage, this, &TelegramAbstractListModel::onUpdateShortChatMessage);
        disconnect(mTelegram.data(), &Telegram::updateShortMessage, this, &TelegramAbstractListModel::onUpdateShortMessage);
        disconnect(mTelegram.data(), &Telegram::updatesGetDifferenceAnswer, this, &TelegramAbstractListModel::onUpdatesGetDifferenceAnswer);
        disconnect(mTelegram.data(), &Telegram::updatesGetStateAnswer, this, &TelegramAbstractListModel::onUpdatesGetStateAnswer);
        disconnect(mTelegram.data(), &Telegram::updatesGetChannelDifferenceAnswer, this, &TelegramAbstractListModel::onUpdatesGetChannelDifferenceAnswer);
    }

    mTelegram = mEngine->telegram();
    if(mTelegram)
    {
        connect(mTelegram.data(), &Telegram::updates, this, &TelegramAbstractListModel::onUpdates);
        connect(mTelegram.data(), &Telegram::updatesCombined, this, &TelegramAbstractListModel::onUpdatesCombined);
        connect(mTelegram.data(), &Telegram::updatesTooLong, this, &TelegramAbstractListModel::onUpdatesTooLong);
        connect(mTelegram.data(), &Telegram::updateSecretChatMessage, this, &TelegramAbstractListModel::onUpdateSecretChatMessage);
        connect(mTelegram.data(), &Telegram::updateShort, this, &TelegramAbstractListModel::onUpdateShort);
        connect(mTelegram.data(), &Telegram::updateShortChatMessage, this, &TelegramAbstractListModel::onUpdateShortChatMessage);
        connect(mTelegram.data(), &Telegram::updateShortMessage, this, &TelegramAbstractListModel::onUpdateShortMessage);
        connect(mTelegram.data(), &Telegram::updatesGetDifferenceAnswer, this, &TelegramAbstractListModel::onUpdatesGetDifferenceAnswer);
        connect(mTelegram.data(), &Telegram::updatesGetStateAnswer, this, &TelegramAbstractListModel::onUpdatesGetStateAnswer);
        connect(mTelegram.data(), &Telegram::updatesGetChannelDifferenceAnswer, this, &TelegramAbstractListModel::onUpdatesGetChannelDifferenceAnswer);
    }
}

TelegramAbstractListModel::~TelegramAbstractListModel()
{
}

