#include "telegramabstractlistmodel.h"

TelegramAbstractListModel::TelegramAbstractListModel(QObject *parent) :
    QAbstractListModel(parent)
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

TelegramAbstractListModel::~TelegramAbstractListModel()
{
}

