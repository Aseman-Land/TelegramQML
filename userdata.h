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

#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QStringList>

#include "telegramqml_global.h"

class TELEGRAMQMLSHARED_EXPORT MessageUpdate
{
public:
    int id;
    QString message;
    qint64 date;
};

class UserDataPrivate;
class TELEGRAMQMLSHARED_EXPORT UserData : public QObject
{
    Q_OBJECT
    Q_ENUMS(NotifyTypes)
    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY configPathChanged)

public:
    enum NotifyTypes {
        NotifyDisabled = 0,
        NotifyOnline = 1,
        NotifyTyping = 2,

        DisableBadges = 4
    };

    UserData(QObject *parent = 0);
    ~UserData();

    void setPhoneNumber(const QString &phoneNumber);
    QString phoneNumber() const;

    void setConfigPath(const QString &path);
    QString configPath() const;

public Q_SLOTS:
    void setPushToken(const QString &token);
    QString pushToken() const;

    void addMute( int id );
    void removeMute( int id );
    QList<int> mutes() const;
    bool isMuted(int id);

    void addFavorite( int id );
    void removeFavorite( int id );
    QList<int> favorites() const;
    bool isFavorited(int id);

    void addLoadLink(int id);
    void removeLoadlink(int id);
    QList<int> loadLinks();
    bool isLoadLink(int id);

    void setNotify(int id, int value);
    int notify(int id);

    void addTag(const QString &tag);
    QStringList tags() const;

    void addMessageUpdate(const MessageUpdate &msg);
    void removeMessageUpdate(int id);
    QList<quint64> messageUpdates() const;
    MessageUpdate messageUpdateItem(int id);

    void setValue( const QString & key, const QString & value );
    QString value( const QString & key ) const;

    void reconnect();
    void disconnect();

Q_SIGNALS:
    void muteChanged(int id);
    void favoriteChanged(int id);
    void loadLinkChanged(int id);
    void tagsChanged(const QString &tag);
    void valueChanged( const QString & key );
    void messageUpdateChanged(int id);
    void notifyChanged(int id, int value);
    void phoneNumberChanged();
    void configPathChanged();

private:
    void refresh();
    void init_buffer();
    void update_db();

private:
    UserDataPrivate *p;
};

Q_DECLARE_METATYPE(UserData*)

#endif // USERDATA_H
