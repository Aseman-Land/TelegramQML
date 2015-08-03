#ifndef STICKERSMODEL_H
#define STICKERSMODEL_H

#include <QAbstractListModel>

class DocumentObject;
class TelegramQml;
class StickersModelPrivate;
class StickersModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool initializing READ initializing NOTIFY initializingChanged)

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

private Q_SLOTS:
    void listChanged();

private:
    class InputStickerSet stickerOfDocument(DocumentObject *obj) const;

private:
    StickersModelPrivate *p;
};

#endif // STICKERSMODEL_H
