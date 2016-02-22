#include "telegramqmlinitializer.h"

#include "telegramengine.h"
#include "telegramapp.h"
#include "telegramauthenticate.h"
#include "telegramdialogslistmodel.h"
#include "telegramhost.h"

#include <telegram/objects/qmltools.h>
#include <qqml.h>

void TelegramQmlInitializer::init(const char *uri)
{
    qtelegramRegisterQmlTypes("TelegramQML");
    qRegisterMetaType< QList<qint32> >("QList<qint32>");

    qmlRegisterType<TelegramEngine>("TelegramQML", 2, 0, "Engine");
    qmlRegisterType<TelegramApp>("TelegramQML", 2, 0, "App");
    qmlRegisterType<TelegramAuthenticate>("TelegramQML", 2, 0, "Authenticate");
    qmlRegisterType<TelegramDialogsListModel>("TelegramQML", 2, 0, "DialogsListModel");
    qmlRegisterType<TelegramHost>("TelegramQML", 2, 0, "Host");

    initializeTypes(uri);
}

void TelegramQmlInitializer::initializeTypes(const char *uri) {
    Q_UNUSED(uri)
}

