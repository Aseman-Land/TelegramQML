#ifndef TELEGRAMENUMS_H
#define TELEGRAMENUMS_H

#include <QObject>

class TelegramEnums : public QObject
{
    Q_OBJECT
    Q_ENUMS(SendFileType)

public:
    enum SendFileType {
        SendFileTypeAutoDetect,
        SendFileTypeDocument,
        SendFileTypeSticker,
        SendFileTypeAnimated,
        SendFileTypeVideo,
        SendFileTypePhoto,
        SendFileTypeAudio
    };

    TelegramEnums(QObject *parent = 0);
    ~TelegramEnums();
};

#endif // TELEGRAMENUMS_H
