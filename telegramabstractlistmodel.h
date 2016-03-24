#ifndef TELEGRAMABSTRACTLISTMODEL_H
#define TELEGRAMABSTRACTLISTMODEL_H

#include "telegramqml_global.h"
#include "telegramtools.h"

#include <QAbstractListModel>
#include <QQmlListProperty>

class TELEGRAMQMLSHARED_EXPORT TelegramAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorChanged)
    Q_PROPERTY(qint32 errorCode READ errorCode NOTIFY errorChanged)
    Q_PROPERTY(QQmlListProperty<QObject> items READ items NOTIFY itemsChanged)
    Q_CLASSINFO("DefaultProperty", "items")

public:
    TelegramAbstractListModel(QObject *parent = 0);
    ~TelegramAbstractListModel();

    Q_INVOKABLE QStringList roles() const;

    int rowCount(const QModelIndex &) const { return count(); }
    virtual int count() const = 0;

    QString errorText() const { return mErrorText; }
    qint32 errorCode() const { return mErrorCode; }

    QQmlListProperty<QObject> items();

public Q_SLOTS:
    QVariant get(int index, int role) const;
    QVariant get(int index, const QString &roleName) const;
    QVariantMap get(int index) const;

Q_SIGNALS:
    void countChanged();
    void errorChanged();
    void itemsChanged();

protected:
    void setError(const QString &errorText, qint32 errorCode) {
        mErrorText = TelegramTools::convertErrorToText(errorText);
        mErrorCode = errorCode;
        Q_EMIT errorChanged();
    }

private:
    static void append(QQmlListProperty<QObject> *p, QObject *v);
    static int count(QQmlListProperty<QObject> *p);
    static QObject *at(QQmlListProperty<QObject> *p, int idx);
    static void clear(QQmlListProperty<QObject> *p);

protected:
    QList<QObject*> _items;

private:
    QString mErrorText;
    qint32 mErrorCode;
};

#endif // TELEGRAMABSTRACTLISTMODEL_H
