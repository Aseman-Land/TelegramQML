#include "telegrammessagesearchmodel.h"
#include "telegramqmlinitializer.h"

#include "telegramengine.h"
#include "telegramapp.h"
#include "telegramauthenticate.h"
#include "telegramdialoglistmodel.h"
#include "telegrammessagelistmodel.h"
#include "telegramprofilemanagermodel.h"
#include "telegrampeerdetails.h"
#include "telegramhost.h"
#include "telegramenums.h"
#include "telegramimageelement.h"
#include "telegrammessagefetcher.h"
#include "telegramstickerscategoriesmodel.h"
#include "telegramstickersmodel.h"
#include "telegramnotificationhandler.h"
#include "tqmldocumentexporter.h"
#include "private/telegramdownloadhandler.h"

#include <telegram/objects/qmltools.h>
#include <qqml.h>

void TelegramQmlInitializer::init(const char *uri)
{
    qtelegramRegisterQmlTypes("TelegramQml", 2, 0);
    qRegisterMetaType< QList<qint32> >("QList<qint32>");

    qmlRegisterUncreatableType<TelegramEnums>("TelegramQml", 2, 0, "Enums", "It's just enums");

    qmlRegisterType<TelegramEngine>("TelegramQml", 2, 0, "Engine");
    qmlRegisterType<TelegramApp>("TelegramQml", 2, 0, "App");
    qmlRegisterType<TelegramAuthenticate>("TelegramQml", 2, 0, "Authenticate");
    qmlRegisterType<TelegramMessageFetcher>("TelegramQml", 2, 0, "MessageFetcher");
    qmlRegisterType<TelegramDialogListModel>("TelegramQml", 2, 0, "DialogListModel");
    qmlRegisterType<TelegramMessageSearchModel>("TelegramQml", 2, 0, "MessageSearchModel");
    qmlRegisterType<TelegramDownloadHandler>("TelegramQml", 2, 0, "DownloadHandler");
    qmlRegisterType<TelegramStickersCategoriesModel>("TelegramQml", 2, 0, "StickersCategoriesModel");
    qmlRegisterType<TelegramStickersModel>("TelegramQml", 2, 0, "StickersModel");
    qmlRegisterType<TelegramMessageListModel>("TelegramQml", 2, 0, "MessageListModel");
    qmlRegisterType<TelegramProfileManagerModel>("TelegramQml", 2, 0, "ProfileManagerModel");
    qmlRegisterType<TelegramPeerDetails>("TelegramQml", 2, 0, "PeerDetails");
    qmlRegisterType<TelegramNotificationHandler>("TelegramQml", 2, 0, "NotificationHandler");
    qmlRegisterType<TelegramHost>("TelegramQml", 2, 0, "Host");
    qmlRegisterType<TelegramImageElement>("TelegramQml", 2, 0, "Image");
    qmlRegisterType<TqmlDocumentExporter>("TelegramQml", 2, 0, "DocumentExporter");

    initializeTypes(uri);
}

void TelegramQmlInitializer::initializeTypes(const char *uri) {
    Q_UNUSED(uri)
}

