#include "telegramprofilemanagermodel.h"

#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QUuid>
#include <QDir>

class TelegramProfileManagerModelItem
{
public:
    QString phoneNumber;
    bool mute;

    bool operator ==(const TelegramProfileManagerModelItem &b) {
        return b.phoneNumber == phoneNumber &&
               b.mute == mute;
    }
};

class TelegramProfileManagerModelPrivate
{
public:
    QList<TelegramProfileManagerModelItem> list;
    QString source;
    QSqlDatabase db;
    QString dbConnection;
};

TelegramProfileManagerModel::TelegramProfileManagerModel(QObject *parent) :
    TelegramAbstractListModel(parent)
{
    p = new TelegramProfileManagerModelPrivate;
    p->dbConnection = QUuid::createUuid().toString();
    p->db = QSqlDatabase::addDatabase("QSQLITE", p->dbConnection);
}

void TelegramProfileManagerModel::init()
{
    if(p->db.isOpen())
        p->db.close();

    if(p->source.isEmpty())
    {
        changed(QList<TelegramProfileManagerModelItem>());
        return;
    }

    p->db.setDatabaseName(p->source);
    if(!p->db.open())
    {
        changed(QList<TelegramProfileManagerModelItem>());
        return;
    }

    initTables();
    initBuffer();
}

void TelegramProfileManagerModel::initTables()
{
    QStringList queries = QStringList() <<
        "CREATE TABLE IF NOT EXISTS Profiles ("
        "phoneNumber VARCHAR(32) NOT NULL,"
        "mute BOOLEAN NOT NULL,"
        "PRIMARY KEY (phoneNumber))";

    Q_FOREACH(const QString &query_str, queries)
    {
        QSqlQuery query(p->db);
        query.prepare(query_str);
        query.exec();
    }
}

void TelegramProfileManagerModel::initBuffer()
{
    QSqlQuery query(p->db);
    query.prepare("SELECT phoneNumber, mute FROM Profiles");
    query.exec();

    QList<TelegramProfileManagerModelItem> list;
    while(query.next())
    {
        QSqlRecord record = query.record();
        TelegramProfileManagerModelItem item;
        item.phoneNumber = record.value("phoneNumber").toString();
        item.mute = record.value("mute").toBool();

        list << item;
    }

    changed(list);
}

TelegramProfileManagerModelItem TelegramProfileManagerModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramProfileManagerModel::count() const
{
    return p->list.count();
}

QVariant TelegramProfileManagerModel::data(const QModelIndex &index, int role) const
{
    const TelegramProfileManagerModelItem &item = id(index);
    QVariant result;
    switch(role)
    {
    case DataPhoneNumber:
        result = item.phoneNumber;
        break;

    case DataMute:
        result = item.mute;
        break;
    }

    return result;
}

QHash<int, QByteArray> TelegramProfileManagerModel::roleNames() const
{
    static QHash<int, QByteArray> *result = 0;
    if(result)
        return *result;

    result = new QHash<int, QByteArray>();
    result->insert(DataPhoneNumber, "phoneNumber");
    result->insert(DataMute, "mute");

    return *result;
}

void TelegramProfileManagerModel::setSource(const QString &source)
{
    if(p->source == source)
        return;

    p->source = source;

    const QString dir = QFileInfo(p->source).dir().path();
    if(!dir.isEmpty())
        QDir().mkpath(dir);

    init();
    Q_EMIT sourceChanged();
}

QString TelegramProfileManagerModel::source() const
{
    return p->source;
}

int TelegramProfileManagerModel::add(const QString &phoneNumber, bool mute)
{
    for(int i=0; i<p->list.count(); i++)
        if(p->list.at(i).phoneNumber == phoneNumber)
            return AddResultExists;
    if(phoneNumber.trimmed().isEmpty())
        return AddResultInvalidPhone;

    QList<TelegramProfileManagerModelItem> list = p->list;
    TelegramProfileManagerModelItem item;
    item.phoneNumber = phoneNumber;
    item.mute = mute;

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO Profiles (phoneNumber, mute) VALUES (:phone, :mute)");
    query.bindValue(":phone", phoneNumber);
    query.bindValue(":mute", mute);
    if(!query.exec())
        return AddResultDatabaseError;

    list << item;
    changed(list);
    return AddResultSucessfully;
}

bool TelegramProfileManagerModel::remove(const QString &phoneNumber)
{
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM Profiles WHERE phoneNumber=:phone");
    query.bindValue(":phone", phoneNumber);
    if(query.exec())
        return false;

    QList<TelegramProfileManagerModelItem> list = p->list;
    for(int i=0; i<list.count(); i++)
    {
        TelegramProfileManagerModelItem item = list.at(i);
        if(item.phoneNumber != phoneNumber)
            continue;

        list.removeAt(i);
        i--;
    }
    changed(list);
    return true;
}

void TelegramProfileManagerModel::changed(const QList<TelegramProfileManagerModelItem> &list)
{
    bool count_changed = (list.count()!=p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const TelegramProfileManagerModelItem &item = p->list.at(i);
        if( list.contains(item) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<TelegramProfileManagerModelItem> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const TelegramProfileManagerModelItem &item = temp_list.at(i);
        if( p->list.contains(item) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const TelegramProfileManagerModelItem &item = p->list.at(i);
            int nw = temp_list.indexOf(item);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const TelegramProfileManagerModelItem &item = list.at(i);
        if( p->list.contains(item) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, item );
        endInsertRows();
    }

    if(count_changed)
        Q_EMIT countChanged();
}

TelegramProfileManagerModel::~TelegramProfileManagerModel()
{
    QString dbConnection = p->dbConnection;
    delete p;
    QSqlDatabase::removeDatabase(dbConnection);
}

