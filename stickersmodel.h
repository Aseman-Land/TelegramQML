#ifndef STICKERSMODEL_H
#define STICKERSMODEL_H

#include <QStringList>

#include "telegramqml_global.h"
#include "tgabstractlistmodel.h"

class DocumentObject;
class StickerSetObject;
class TelegramQml;
class StickersModelPrivate;
class TELEGRAMQMLSHARED_EXPORT StickersModel : public TgAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(DialogsRoles)

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool initializing READ initializing NOTIFY initializingChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)

public:
    enum DialogsRoles {
        stickerIdRole = Qt::UserRole,
        documentRole,
        emoticonRole,
        stickerSetNameRole,
        stickerSetIdRole,
        stickerSetAccessHashRole
    };

    StickersModel(QObject *parent = 0);
    ~StickersModel();

    TelegramQml *telegram() const;
    void setTelegram(TelegramQml *tgo );

    QString category() const;
    void setCategory(const QString &cat);

    QStringList categories() const;
    Q_INVOKABLE DocumentObject *categoryThumbnailDocument(const QString &id) const;
    Q_INVOKABLE StickerSetObject *categoryItem(const QString &id) const;

    qint64 id( const QModelIndex &index ) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;

    int count() const;
    bool initializing() const;

public Q_SLOTS:
    void refresh();

Q_SIGNALS:
    void telegramChanged();
    void countChanged();
    void initializingChanged();
    void categoryChanged();
    void categoriesChanged();

private Q_SLOTS:
    void recheck();
    void listChanged(bool cached = false);

private:
    class InputStickerSet stickerOfDocument(DocumentObject *obj) const;

private:
    StickersModelPrivate *p;
};

#endif // STICKERSMODEL_H
