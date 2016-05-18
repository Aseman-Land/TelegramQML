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
    qtelegramRegisterQmlTypes(uri, 2, 0);
    qRegisterMetaType< QList<qint32> >("QList<qint32>");

    qmlRegisterUncreatableType<TelegramEnums>(uri, 2, 0, "Enums", "It's just enums");

    qmlRegisterType<TelegramEngine>(uri, 2, 0, "Engine");
    qmlRegisterType<TelegramApp>(uri, 2, 0, "App");
    qmlRegisterType<TelegramAuthenticate>(uri, 2, 0, "Authenticate");
    qmlRegisterType<TelegramMessageFetcher>(uri, 2, 0, "MessageFetcher");
    qmlRegisterType<TelegramDialogListModel>(uri, 2, 0, "DialogListModel");
    qmlRegisterType<TelegramMessageSearchModel>(uri, 2, 0, "MessageSearchModel");
    qmlRegisterType<TelegramDownloadHandler>(uri, 2, 0, "DownloadHandler");
    qmlRegisterType<TelegramStickersCategoriesModel>(uri, 2, 0, "StickersCategoriesModel");
    qmlRegisterType<TelegramStickersModel>(uri, 2, 0, "StickersModel");
    qmlRegisterType<TelegramMessageListModel>(uri, 2, 0, "MessageListModel");
    qmlRegisterType<TelegramProfileManagerModel>(uri, 2, 0, "ProfileManagerModel");
    qmlRegisterType<TelegramPeerDetails>(uri, 2, 0, "PeerDetails");
    qmlRegisterType<TelegramNotificationHandler>(uri, 2, 0, "NotificationHandler");
    qmlRegisterType<TelegramHost>(uri, 2, 0, "Host");
    qmlRegisterType<TelegramImageElement>(uri, 2, 0, "Image");
    qmlRegisterType<TqmlDocumentExporter>(uri, 2, 0, "DocumentExporter");

    initializeTypes(uri);
}

void TelegramQmlInitializer::initializeTypes(const char *uri) {
    Q_UNUSED(uri)
}

