#ifndef TELEGRAMPEERDETAILS_H
#define TELEGRAMPEERDETAILS_H

#include "tqobject.h"

class TelegramPeerDetailsPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramPeerDetails : public TqObject
{
    Q_OBJECT
public:
    TelegramPeerDetails(QObject *parent = 0);
    ~TelegramPeerDetails();

Q_SIGNALS:

public Q_SLOTS:

private:
    TelegramPeerDetailsPrivate *p;
};

#endif // TELEGRAMPEERDETAILS_H
