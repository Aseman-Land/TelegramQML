#define READ_PROPERTY(NAME, TYPE) \
    initImage(); \
    return p->image->property(#NAME).value<TYPE>();

#define SET_PROPERTY(NAME, TYPE) \
    initImage(); \
    p->image->setProperty(#NAME, QVariant::fromValue<TYPE>(NAME));

#include "telegramimageelement.h"
#include "telegramdownloadhandler.h"
#include "telegramengine.h"

#include <telegram/objects/typeobjects.h>

#include <QDebug>
#include <QtQml>
#include <QPointer>
#include <QMimeDatabase>
#include <QMimeType>
#include <QFileInfo>

class TelegramImageElementPrivate
{
public:
    TelegramDownloadHandler *handler;
    QQuickItem *image;
    QMimeDatabase mdb;
};

TelegramImageElement::TelegramImageElement(QQuickItem *parent) :
    QQuickItem(parent)
{
    p = new TelegramImageElementPrivate;
    p->image = 0;

    p->handler = new TelegramDownloadHandler(this);

    connect(p->handler, &TelegramDownloadHandler::sourceChanged       , this, &TelegramImageElement::sourceChanged       );
    connect(p->handler, &TelegramDownloadHandler::engineChanged       , this, &TelegramImageElement::engineChanged       );
    connect(p->handler, &TelegramDownloadHandler::sizeChanged         , this, &TelegramImageElement::fileSizeChanged     );
    connect(p->handler, &TelegramDownloadHandler::downloadTotalChanged, this, &TelegramImageElement::fileSizeChanged     );
    connect(p->handler, &TelegramDownloadHandler::dowloadedSizeChanged, this, &TelegramImageElement::dowloadedSizeChanged);
    connect(p->handler, &TelegramDownloadHandler::downloadingChanged  , this, &TelegramImageElement::downloadingChanged  );
    connect(p->handler, &TelegramDownloadHandler::destinationChanged  , this, &TelegramImageElement::destinationChangeEvent);
    connect(p->handler, &TelegramDownloadHandler::thumbnailChanged    , this, &TelegramImageElement::thumbnailChangeEvent);
}

void TelegramImageElement::setSource(TelegramTypeQObject *object)
{
    p->handler->setSource(object);
}

TelegramTypeQObject *TelegramImageElement::source() const
{
    return p->handler->source();
}

void TelegramImageElement::setEngine(TelegramEngine *engine)
{
    p->handler->setEngine(engine);
}

TelegramEngine *TelegramImageElement::engine() const
{
    return p->handler->engine();
}

bool TelegramImageElement::asynchronous()
{
    READ_PROPERTY(asynchronous, bool);
}

void TelegramImageElement::setAsynchronous(bool asynchronous)
{
    SET_PROPERTY(asynchronous, bool);
}

bool TelegramImageElement::autoTransform()
{
    READ_PROPERTY(autoTransform, bool);
}

void TelegramImageElement::setAutoTransform(bool autoTransform)
{
    SET_PROPERTY(autoTransform, bool);
}

bool TelegramImageElement::cache()
{
    READ_PROPERTY(cache, bool);
}

void TelegramImageElement::setCache(bool cache)
{
    SET_PROPERTY(cache, bool);
}

int TelegramImageElement::fillMode()
{
    READ_PROPERTY(fillMode, int);
}

void TelegramImageElement::setFillMode(int fillMode)
{
    SET_PROPERTY(fillMode, bool);
}

int TelegramImageElement::horizontalAlignment()
{
    READ_PROPERTY(horizontalAlignment, int);
}

void TelegramImageElement::setHorizontalAlignment(int horizontalAlignment)
{
    SET_PROPERTY(horizontalAlignment, int);
    Q_EMIT horizontalAlignmentChanged();
}

bool TelegramImageElement::mipmap()
{
    READ_PROPERTY(mipmap, bool);
}

void TelegramImageElement::setMipmap(bool mipmap)
{
    SET_PROPERTY(mipmap, bool);
    Q_EMIT mipmapChanged();
}

bool TelegramImageElement::mirror()
{
    READ_PROPERTY(mirror, bool);
}

void TelegramImageElement::setMirror(bool mirror)
{
    SET_PROPERTY(mirror, bool);
}

qreal TelegramImageElement::paintedHeight()
{
    READ_PROPERTY(paintedHeight, qreal);
}

qreal TelegramImageElement::paintedWidth()
{
    READ_PROPERTY(paintedWidth, qreal);
}

bool TelegramImageElement::smooth()
{
    READ_PROPERTY(smooth, qreal);
}

void TelegramImageElement::setSmooth(bool smooth)
{
    SET_PROPERTY(smooth, qreal);
    Q_EMIT smoothChanged();
}

QSizeF TelegramImageElement::sourceSize()
{
    READ_PROPERTY(sourceSize, QSizeF);
}

void TelegramImageElement::setSourceSize(const QSizeF &sourceSize)
{
    SET_PROPERTY(sourceSize, QSizeF);
}

int TelegramImageElement::verticalAlignment()
{
    READ_PROPERTY(verticalAlignment, int);
}

void TelegramImageElement::setVerticalAlignment(int verticalAlignment)
{
    SET_PROPERTY(verticalAlignment, int);
    Q_EMIT verticalAlignmentChanged();
}

qint32 TelegramImageElement::fileSize() const
{
    return p->handler->downloadTotal()? p->handler->downloadTotal() : p->handler->size();
}

qint32 TelegramImageElement::dowloadedSize() const
{
    return p->handler->dowloadedSize();
}

bool TelegramImageElement::downloading() const
{
    return p->handler->downloading();
}

QUrl TelegramImageElement::destination() const
{
    QString path = p->handler->destination();
    if(path.isEmpty())
        return QUrl();
    else
        return QUrl::fromLocalFile(path);
}

QUrl TelegramImageElement::thumbnail() const
{
    QString path = p->handler->thumbnail();
    if(path.isEmpty())
        return QUrl();
    else
        return QUrl::fromLocalFile(path);
}

bool TelegramImageElement::download()
{
    return p->handler->download();
}

bool TelegramImageElement::check()
{
    return p->handler->check();
}

void TelegramImageElement::initImage()
{
    if(p->image)
        return;

    QQmlEngine *engine = qmlEngine(this);
    QQmlContext *context = qmlContext(this);
    if(!engine || !context)
        return;

    QQmlComponent component(engine);
    component.setData("import QtQuick 2.5\n"
                      "Image { anchors.fill: parent; }",
                      QUrl());
    QQuickItem *item = qobject_cast<QQuickItem *>(component.create(context));
    if(!item)
        return;

    item->setParent(this);
    item->setParentItem(this);

    connect(item, SIGNAL(asynchronousChanged()), this, SIGNAL(asynchronousChanged()));
    connect(item, SIGNAL(autoTransformChanged()), this, SIGNAL(autoTransformChanged()));
    connect(item, SIGNAL(cacheChanged()), this, SIGNAL(cacheChanged()));
    connect(item, SIGNAL(fillModeChanged()), this, SIGNAL(fillModeChanged()));
    connect(item, SIGNAL(mirrorChanged()), this, SIGNAL(mirrorChanged()));
    connect(item, SIGNAL(sourceSizeChanged()), this, SIGNAL(sourceSizeChanged()));

    p->image = item;
}

void TelegramImageElement::setImage(const QString &image)
{
    initImage();
    p->image->setProperty("source", QUrl::fromLocalFile(image));
}

void TelegramImageElement::destinationChangeEvent()
{
    QString dest = p->handler->destination();
    if(dest.isEmpty() || !p->mdb.mimeTypeForFile(dest).name().contains("image", Qt::CaseInsensitive))
        thumbnailChanged();
    else
        setImage(dest);

    Q_EMIT destinationChanged();
}

void TelegramImageElement::thumbnailChangeEvent()
{
    QString thumb = p->handler->thumbnail();
    if(!thumb.isEmpty())
        setImage(thumb);

    Q_EMIT thumbnailChanged();
}

TelegramImageElement::~TelegramImageElement()
{
    delete p;
}

