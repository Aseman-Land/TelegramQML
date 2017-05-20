#ifndef TELEGRAMQML_PLUGIN_H
#define TELEGRAMQML_PLUGIN_H

#include <QQmlExtensionPlugin>

class TelegramQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri);
};

#endif // TELEGRAMQML_PLUGIN_H

