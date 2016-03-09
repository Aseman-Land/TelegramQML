#ifndef TELEGRAMABSTRACTLISTMODEL_H
#define TELEGRAMABSTRACTLISTMODEL_H

#include "telegramqml_global.h"
#include <QAbstractListModel>

class TELEGRAMQMLSHARED_EXPORT TelegramAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    TelegramAbstractListModel(QObject *parent = 0);
    ~TelegramAbstractListModel();

    Q_INVOKABLE QStringList roles() const;

    int rowCount(const QModelIndex &) const { return count(); }
    virtual int count() const = 0;

public Q_SLOTS:
    QVariant get(int index, int role) const;
    QVariant get(int index, const QString &roleName) const;
    QVariantMap get(int index) const;

Q_SIGNALS:
    void countChanged();
};

#endif // TELEGRAMABSTRACTLISTMODEL_H
