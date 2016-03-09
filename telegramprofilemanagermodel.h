#ifndef TELEGRAMPROFILEMANAGERMODEL_H
#define TELEGRAMPROFILEMANAGERMODEL_H

#include "telegramabstractlistmodel.h"
#include "telegramqml_global.h"

#include <QStringList>

class TelegramProfileManagerModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramProfileManagerModel : public TelegramAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(DataRole)
    Q_ENUMS(AddResult)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

public:
    enum DataRole {
        DataPhoneNumber = Qt::UserRole,
        DataMute
    };

    enum AddResult {
        AddResultSucessfully,
        AddResultExists,
        AddResultInvalidPhone,
        AddResultDatabaseError
    };

    TelegramProfileManagerModel(QObject *parent = 0);
    ~TelegramProfileManagerModel();

    class TelegramProfileManagerModelItem id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    void setSource(const QString &source);
    QString source() const;

public Q_SLOTS:
    int add(const QString &phoneNumber, bool mute);
    bool remove(const QString &phoneNumber);

Q_SIGNALS:
    void sourceChanged();

private:
    void changed(const QList<class TelegramProfileManagerModelItem> &list);
    void init();
    void initTables();
    void initBuffer();

private:
    TelegramProfileManagerModelPrivate *p;
};

#endif // TELEGRAMPROFILEMANAGERMODEL_H
