#include "telegramqmlinitializer.h"

#include "telegramengine.h"
#include "telegramapp.h"
#include "telegramauthenticate.h"
#include "telegramdialoglistmodel.h"
#include "telegrammessagelistmodel.h"
#include "telegramprofilemanagermodel.h"
#include "telegramhost.h"
#include "telegramenums.h"

#include <telegram/objects/qmltools.h>
#include <qqml.h>

void TelegramQmlInitializer::init(const char *uri)
{
    qtelegramRegisterQmlTypes("TelegramQML");
    qRegisterMetaType< QList<qint32> >("QList<qint32>");

    qmlRegisterUncreatableType<TelegramEnums>("TelegramQML", 2, 0, "Enums", "It's just enums");

    qmlRegisterType<TelegramEngine>("TelegramQML", 2, 0, "Engine");
    qmlRegisterType<TelegramApp>("TelegramQML", 2, 0, "App");
    qmlRegisterType<TelegramAuthenticate>("TelegramQML", 2, 0, "Authenticate");
    qmlRegisterType<TelegramDialogListModel>("TelegramQML", 2, 0, "DialogListModel");
    qmlRegisterType<TelegramMessageListModel>("TelegramQML", 2, 0, "MessageListModel");
    qmlRegisterType<TelegramProfileManagerModel>("TelegramQML", 2, 0, "ProfileManagerModel");
    qmlRegisterType<TelegramHost>("TelegramQML", 2, 0, "Host");

    initializeTypes(uri);
}

void TelegramQmlInitializer::initializeTypes(const char *uri) {
    Q_UNUSED(uri)
}

