/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    Cutegram is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cutegram is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "userdata.h"
#include "telegramqml_macros.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDateTime>
#include <QSettings>
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QUuid>

class SecretChatDBClass
{
public:
    int id;
    int userId;
    QString title;
};

class UserDataPrivate
{
public:
    QString connectionName;
    QSqlDatabase db;
    QString path;

    QString phoneNumber;
    QString configPath;

    QHash<int,bool> mutes;
    QHash<int,bool> favorites;
    QHash<int,bool> loadLink;
    QHash<QString,QString> general;
    QMap<quint64, MessageUpdate> msg_updates;
    QMap<QString,bool> tags;
    QHash<int,int> notifies;
};

UserData::UserData(QObject *parent) :
    QObject(parent)
{
    p = new UserDataPrivate;
    p->connectionName = USERDATA_DB_CONNECTION + p->phoneNumber + QUuid::createUuid().toString();
}

void UserData::setPhoneNumber(const QString &phoneNumber)
{
    if(p->phoneNumber == phoneNumber)
        return;

    p->phoneNumber = phoneNumber;
    refresh();

    Q_EMIT phoneNumberChanged();
}

QString UserData::phoneNumber() const
{
    return p->phoneNumber;
}

void UserData::setConfigPath(const QString &path)
{
    if(p->configPath == path)
        return;

    p->configPath = path;
    refresh();

    Q_EMIT configPathChanged();
}

QString UserData::configPath() const
{
    return p->configPath;
}

void UserData::disconnect()
{
    p->db.close();
}

void UserData::refresh()
{
    if(p->phoneNumber.isEmpty() || p->configPath.isEmpty())
    {
        disconnect();
        return;
    }

    const QString &dirPath = p->configPath + "/" + p->phoneNumber;
    QDir().mkpath(dirPath);

    p->path = dirPath + "/userdata.db";

    if( !QFileInfo::exists(p->path) )
        QFile::copy(USERDATA_DB_PATH,p->path);

    QFile(p->path).setPermissions(QFileDevice::WriteOwner|QFileDevice::WriteGroup|QFileDevice::ReadUser|QFileDevice::ReadGroup);

    p->db = QSqlDatabase::addDatabase("QSQLITE",p->connectionName);
    p->db.setDatabaseName(p->path);

    reconnect();
}

void UserData::reconnect()
{
    p->db.open();
    init_buffer();
    update_db();
}

// WARNING: Push notifications supported for one account only!
void UserData::setPushToken(const QString &token)
{
    QString key = QString("push%1").arg(p->phoneNumber);
    setValue(key, token);
}

QString UserData::pushToken() const
{
    QString key = QString("push%1").arg(p->phoneNumber);
    return value(key);
}

void UserData::addMute(int id)
{
    QSqlQuery mute_query(p->db);
    mute_query.prepare("INSERT OR REPLACE INTO mutes (id,mute) VALUES (:id,:mute)");
    mute_query.bindValue(":id",id);
    mute_query.bindValue(":mute",1);
    mute_query.exec();
    CHECK_QUERY_ERROR(mute_query);

    p->mutes.insert(id,true);
    Q_EMIT muteChanged(id);
}

void UserData::removeMute(int id)
{
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM mutes WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    CHECK_QUERY_ERROR(query);

    p->mutes.remove(id);
    Q_EMIT muteChanged(id);
}

QList<int> UserData::mutes() const
{
    QList<int> res;
    QHashIterator<int,bool> i(p->mutes);
    while( i.hasNext() )
    {
        i.next();
        if( i.value() )
            res << i.key();
    }

    return res;
}

bool UserData::isMuted(int id)
{
    return p->mutes.value(id);
}

void UserData::addFavorite(int id)
{
    QSqlQuery mute_query(p->db);
    mute_query.prepare("INSERT OR REPLACE INTO favorites (id,favorite) VALUES (:id,:fave)");
    mute_query.bindValue(":id",id);
    mute_query.bindValue(":fave",1);
    mute_query.exec();
    CHECK_QUERY_ERROR(mute_query);

    p->favorites.insert(id,true);
    Q_EMIT favoriteChanged(id);
}

void UserData::removeFavorite(int id)
{
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM favorites WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();

    p->favorites.remove(id);
    Q_EMIT favoriteChanged(id);
}

QList<int> UserData::favorites() const
{
    QList<int> res;
    QHashIterator<int,bool> i(p->favorites);
    while( i.hasNext() )
    {
        i.next();
        if( i.value() )
            res << i.key();
    }

    return res;
}

bool UserData::isFavorited(int id)
{
    return p->favorites.value(id);
}

void UserData::addLoadLink(int id)
{
    QSqlQuery mute_query(p->db);
    mute_query.prepare("INSERT OR REPLACE INTO loadLink (id,canLoad) VALUES (:id,:cld)");
    mute_query.bindValue(":id",id);
    mute_query.bindValue(":cld",1);
    mute_query.exec();
    CHECK_QUERY_ERROR(mute_query);

    p->loadLink.insert(id,true);
    Q_EMIT loadLinkChanged(id);
}

void UserData::removeLoadlink(int id)
{
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM loadLink WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();

    p->loadLink.remove(id);
    Q_EMIT loadLinkChanged(id);
}

QList<int> UserData::loadLinks()
{
    QList<int> res;
    QHashIterator<int,bool> i(p->loadLink);
    while( i.hasNext() )
    {
        i.next();
        if( i.value() )
            res << i.key();
    }

    return res;
}

bool UserData::isLoadLink(int id)
{
    return p->loadLink.value(id);
}

void UserData::setNotify(int id, int value)
{
    QSqlQuery notify_query(p->db);
    notify_query.prepare("INSERT OR REPLACE INTO notifysettings (id,value) VALUES (:id,:val)");
    notify_query.bindValue(":id",id);
    notify_query.bindValue(":val",value);
    notify_query.exec();
    CHECK_QUERY_ERROR(notify_query);

    p->notifies.insert(id,value);
    Q_EMIT notifyChanged(id, value);
}

int UserData::notify(int id)
{
    return p->notifies.value(id);
}

void UserData::addTag(const QString &t)
{
    const QString &tag = t.toLower();
    if(p->tags.contains(tag))
        return;

    QSqlQuery mute_query(p->db);
    mute_query.prepare("INSERT OR REPLACE INTO tags (tag) VALUES (:tag)");
    mute_query.bindValue(":tag",tag);
    mute_query.exec();
    CHECK_QUERY_ERROR(mute_query);

    p->tags.insert(tag, true);
    Q_EMIT tagsChanged(tag);
}

QStringList UserData::tags() const
{
    return p->tags.keys();
}

void UserData::addMessageUpdate(const MessageUpdate &msg)
{
    QSqlQuery mute_query(p->db);
    mute_query.prepare("INSERT OR REPLACE INTO updatemessages (id, message, date) VALUES (:id, :msg, :date)");
    mute_query.bindValue(":id"  ,msg.id);
    mute_query.bindValue(":msg" ,msg.message);
    mute_query.bindValue(":date",msg.date);
    mute_query.exec();
    CHECK_QUERY_ERROR(mute_query);

    p->msg_updates[msg.id] = msg;
    Q_EMIT messageUpdateChanged(msg.id);
}

void UserData::removeMessageUpdate(int id)
{
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM updatemessages WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    CHECK_QUERY_ERROR(query);

    p->msg_updates.remove(id);
    Q_EMIT messageUpdateChanged(id);
}

QList<quint64> UserData::messageUpdates() const
{
    return p->msg_updates.keys();
}

MessageUpdate UserData::messageUpdateItem(int id)
{
    return p->msg_updates.value(id);
}

void UserData::setValue(const QString &key, const QString &value)
{
    QSqlQuery mute_query(p->db);
    mute_query.prepare("INSERT OR REPLACE INTO general (gkey,gvalue) VALUES (:key,:val)");
    mute_query.bindValue(":key", key);
    mute_query.bindValue(":val", value);
    mute_query.exec();
    CHECK_QUERY_ERROR(mute_query);

    p->general[key] = value;
    Q_EMIT valueChanged(key);
}

QString UserData::value(const QString &key) const
{
    return p->general.value(key);
}

void UserData::init_buffer()
{
    p->mutes.clear();
    p->favorites.clear();
    p->msg_updates.clear();
    p->general.clear();

    QSqlQuery mute_query(p->db);
    mute_query.prepare("SELECT id, mute FROM mutes");
    mute_query.exec();

    while( mute_query.next() )
    {
        const QSqlRecord & record = mute_query.record();
        p->mutes.insert( record.value(0).toInt(), record.value(1).toInt() );
    }

    QSqlQuery faves_query(p->db);
    faves_query.prepare("SELECT id, favorite FROM favorites");
    faves_query.exec();

    while( faves_query.next() )
    {
        const QSqlRecord & record = faves_query.record();
        p->favorites.insert( record.value(0).toInt(), record.value(1).toInt() );
    }

    QSqlQuery llinks_query(p->db);
    llinks_query.prepare("SELECT id, canLoad FROM loadLink");
    llinks_query.exec();

    while( llinks_query.next() )
    {
        const QSqlRecord & record = llinks_query.record();
        p->loadLink.insert( record.value(0).toInt(), record.value(1).toInt() );
    }

    QSqlQuery notifies_query(p->db);
    notifies_query.prepare("SELECT id, value FROM notifysettings");
    notifies_query.exec();

    while( notifies_query.next() )
    {
        const QSqlRecord & record = notifies_query.record();
        p->notifies.insert( record.value(0).toInt(), record.value(1).toInt() );
    }

    QSqlQuery tags_query(p->db);
    tags_query.prepare("SELECT tag FROM tags");
    tags_query.exec();

    while( tags_query.next() )
    {
        const QSqlRecord & record = tags_query.record();
        p->tags.insert( record.value(0).toString(), true );
    }

    QSqlQuery msg_upd_query(p->db);
    msg_upd_query.prepare("SELECT id, message, date FROM updatemessages");
    msg_upd_query.exec();

    while( msg_upd_query.next() )
    {
        const QSqlRecord & record = msg_upd_query.record();
        MessageUpdate msg;
        msg.id = record.value(0).toULongLong();
        msg.message = record.value(1).toString();
        msg.date = record.value(2).toLongLong();

        p->msg_updates[msg.id] = msg;
    }

    QSqlQuery general_query(p->db);
    general_query.prepare("SELECT gkey, gvalue FROM general");
    general_query.exec();

    while( general_query.next() )
    {
        const QSqlRecord & record = general_query.record();
        p->general.insert( record.value(0).toString(), record.value(1).toString() );
    }
}

void UserData::update_db()
{
    const int version = value("version").toInt();
    if( version < 3 )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "CREATE  TABLE IF NOT EXISTS UpdateMessages (id BIGINT NOT NULL ,message TEXT NOT NULL, date BIGINT NOT NULL, PRIMARY KEY (id) );";
        query_list << "COMMIT;";

        Q_FOREACH( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","3");
    }
    if( version < 4 )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "CREATE  TABLE IF NOT EXISTS Tags (tag TEXT NOT NULL, PRIMARY KEY (tag) );";
        query_list << "COMMIT;";

        Q_FOREACH( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","4");
    }
    if( version < 5 )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "CREATE  TABLE IF NOT EXISTS NotifySettings (id INT NOT NULL ,value INT NOT NULL, PRIMARY KEY (id) );";
        query_list << "COMMIT;";

        Q_FOREACH( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","5");
    }
    if( version < 6 )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "CREATE  TABLE IF NOT EXISTS loadLink (id INT NOT NULL ,canLoad INT NOT NULL, PRIMARY KEY (id) );";
        query_list << "COMMIT;";

        Q_FOREACH( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","6");
    }
}

UserData::~UserData()
{
    QString connectionName = p->connectionName;
    delete p;
    if(QSqlDatabase::contains(connectionName))
        QSqlDatabase::removeDatabase(connectionName);
}
