#ifndef TELEGRAMAPP_H
#define TELEGRAMAPP_H

#include <QObject>

#include "telegramqml_macros.h"
#include "telegramqml_global.h"

/*!
 * Define and Keep a Telegram App details
 * to use in the Telegram.Engine component.
 */
class TelegramAppPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint32 appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(QString appHash READ appHash WRITE setAppHash NOTIFY appHashChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)

public:
    TelegramApp(QObject *parent = 0);
    ~TelegramApp();

    void setAppId(const qint32 &appId);
    qint32 appId() const;

    void setAppHash(const QString &appHash);
    QString appHash() const;

    virtual bool isValid() const;

Q_SIGNALS:
    void appIdChanged();
    void appHashChanged();
    void isValidChanged();

protected:
    void refreshValid();

private:
    TelegramAppPrivate *p;
};

#endif // TELEGRAMAPP_H
