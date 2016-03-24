#define DEFINE_DIS \
    QPointer<TelegramFileLocation> dis = this;

#include "telegramfilelocation.h"
#include "telegramengine.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QPointer>
#include <QFile>
#include <QFileInfo>
#include <QDir>

class TelegramFileLocationPrivate
{
public:
    qint32 dcId;
    qint32 size;
    qint32 dowloadedSize;
    qint32 downloadTotal;
    int classType;
    InputFileLocationObject *location;
    QPointer<TelegramEngine> engine;
    bool downloading;
    QString destination;
};

TelegramFileLocation::TelegramFileLocation(TelegramEngine *engine) :
    TqObject(engine)
{
    p = new TelegramFileLocationPrivate;
    p->dcId = 0;
    p->size = 0;
    p->dowloadedSize = 0;
    p->downloadTotal = 0;
    p->downloading = false;
    p->engine = engine;
    p->classType = TypeFileLocationEmpty;

    p->location = new InputFileLocationObject(this);

    connect(p->location, &InputFileLocationObject::accessHashChanged, this, &TelegramFileLocation::accessHashChanged);
    connect(p->location, &InputFileLocationObject::localIdChanged   , this, &TelegramFileLocation::localIdChanged   );
    connect(p->location, &InputFileLocationObject::secretChanged    , this, &TelegramFileLocation::secretChanged    );
    connect(p->location, &InputFileLocationObject::volumeIdChanged  , this, &TelegramFileLocation::volumeIdChanged  );
    connect(p->location, &InputFileLocationObject::idChanged        , this, &TelegramFileLocation::idChanged        );
    connect(p->location, &InputFileLocationObject::coreChanged      , this, &TelegramFileLocation::locationChanged  );

    connect(p->engine.data(), &TelegramEngine::destroyed, this, &TelegramFileLocation::deleteLater);
}

qint32 TelegramFileLocation::dcId() const
{
    return p->dcId;
}

void TelegramFileLocation::setDcId(const qint32 &dcId)
{
    if(p->dcId == dcId)
        return;

    p->dcId = dcId;
    Q_EMIT dcIdChanged();
}

qint32 TelegramFileLocation::localId() const
{
    return p->location->localId();
}

void TelegramFileLocation::setLocalId(const qint32 &localId)
{
    p->location->setLocalId(localId);
}

qint64 TelegramFileLocation::secret() const
{
    return p->location->secret();
}

void TelegramFileLocation::setSecret(const qint64 &secret)
{
    p->location->setSecret(secret);
}

qint64 TelegramFileLocation::volumeId() const
{
    return p->location->volumeId();
}

void TelegramFileLocation::setVolumeId(const qint64 &volumeId)
{
    p->location->setVolumeId(volumeId);
}

qint64 TelegramFileLocation::accessHash() const
{
    return p->location->accessHash();
}

void TelegramFileLocation::setAccessHash(const qint64 &accessHash)
{
    p->location->setAccessHash(accessHash);
}

qint64 TelegramFileLocation::id() const
{
    return p->location->id();
}

void TelegramFileLocation::setId(const qint64 &id)
{
    p->location->setId(id);
}

qint32 TelegramFileLocation::size() const
{
    return p->size;
}

void TelegramFileLocation::setSize(const qint32 &size)
{
    if(p->size == size)
        return;

    p->size = size;
    Q_EMIT sizeChanged();
}

qint32 TelegramFileLocation::dowloadedSize() const
{
    return p->dowloadedSize;
}

void TelegramFileLocation::setDownloadedSize(qint32 size)
{
    if(p->dowloadedSize == size)
        return;

    p->dowloadedSize = size;
    Q_EMIT dowloadedSizeChanged();
}

qint32 TelegramFileLocation::downloadTotal() const
{
    return p->downloadTotal;
}

bool TelegramFileLocation::downloading() const
{
    return p->downloading;
}

void TelegramFileLocation::setDownloading(bool downloading)
{
    if(p->downloading == downloading)
        return;

    p->downloading = downloading;
    Q_EMIT downloadingChanged();
}

void TelegramFileLocation::setDownloadTotal(qint32 total)
{
    if(p->downloadTotal == total)
        return;

    p->downloadTotal = total;
    Q_EMIT downloadTotalChanged();
}

TelegramEngine *TelegramFileLocation::engine() const
{
    return p->engine;
}

int TelegramFileLocation::classType() const
{
    return p->classType;
}

void TelegramFileLocation::setClassType(int classType)
{
    if(p->classType == classType)
        return;

    p->classType = classType;
    Q_EMIT classTypeChanged();
}

InputFileLocationObject *TelegramFileLocation::location() const
{
    return p->location;
}

QString TelegramFileLocation::destination() const
{
    return p->destination;
}

void TelegramFileLocation::setDestination(const QString &destination)
{
    p->destination = destination;
    Q_EMIT destinationChanged();
}

QString TelegramFileLocation::getLocation() const
{
    if(p->engine->configDirectory().isEmpty() || p->engine->phoneNumber().isEmpty())
        return QString();

    const QString profilePath = p->engine->configDirectory() + "/" + p->engine->phoneNumber() + "/downloads/";
    QDir().mkpath(profilePath);

    return profilePath + p->location->core().getHash().toHex();
}

bool TelegramFileLocation::download()
{
    if(!p->engine || !p->engine->telegram())
        return false;
    if(p->downloading)
        return true;

    const QString location = getLocation();
    if(QFileInfo::exists(location))
    {
        if(size() && QFileInfo(location).size() != size())
        {
            QFile::remove(location);
        }
        else
        {
            setDownloadTotal(size());
            setDownloadedSize(downloadTotal());
            setDestination(location);
            Q_EMIT finished();
            return true;
        }
    }

    QPointer<QFile> file = new QFile(location, this);
    if(!file->open(QFile::WriteOnly))
    {
        delete file;
        return false;
    }

    setDownloadTotal(size());
    setDownloading(true);

    DEFINE_DIS;
    Telegram *tg = p->engine->telegram();
    tg->uploadGetFile(p->location->core(), p->size, p->dcId, [this, dis, file](TG_UPLOAD_GET_FILE_CUSTOM_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis || !file)
            return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            setDownloadedSize(0);
            setDownloadTotal(0);
            setDownloading(false);
            file->close();
            file->remove();
            delete file;
            Q_EMIT finished();
            return;
        }

        switch(static_cast<int>(result.classType()))
        {
        case UploadGetFile::typeUploadGetFileEmpty:
        case UploadGetFile::typeUploadGetFileCanceled:
            setDownloadedSize(0);
            setDownloadTotal(0);
            setDownloading(false);
            file->close();
            file->remove();
            delete file;
            Q_EMIT finished();
            break;

        case UploadGetFile::typeUploadGetFileFinished:
            setDownloadTotal(result.total());
            setDownloadedSize(downloadTotal());
            setDownloading(false);
            file->write(result.bytes());
            file->close();
            setDestination(file->fileName());
            delete file;
            Q_EMIT finished();
            break;

        case UploadGetFile::typeUploadGetFileProgress:
            file->write(result.bytes());
            setDownloadTotal(result.total());
            setDownloadedSize(result.downloaded());
            break;
        }
    });

    return true;
}

bool TelegramFileLocation::check()
{
    if(!p->engine || !p->engine->telegram())
        return false;
    if(p->downloading)
        return false;

    const QString location = getLocation();
    if(QFileInfo::exists(location))
    {
        if(size() && QFileInfo(location).size() != size())
            return false;
        else
            return true;
    }
    else
        return false;
}

void TelegramFileLocation::calculateDestination()
{
    if(!p->engine || !p->engine->isValid())
        return;

    switch(p->classType)
    {
    case TypeFileLocationDocument:
        break;
    case TypeFileLocationPhoto:
        break;
    case TypeFileLocationEncrypted:
        break;
    }
}

TelegramFileLocation::~TelegramFileLocation()
{
    delete p;
}
