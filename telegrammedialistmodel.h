#ifndef TELEGRAMMEDIALISTMODEL_H
#define TELEGRAMMEDIALISTMODEL_H

#include "telegrammessagesearchmodel.h"

class TelegramMediaListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramMediaListModel : public TelegramMessageSearchModel
{
    Q_OBJECT

public:
    TelegramMediaListModel(QObject *parent = 0);
    virtual ~TelegramMediaListModel();

    static QStringList requiredProperties();

private:
    TelegramMediaListModelPrivate *p;
};

#endif // TELEGRAMMEDIALISTMODEL_H
