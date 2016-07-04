/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TELEGRAMDIALOGSMODEL_H
#define TELEGRAMDIALOGSMODEL_H

#include "telegramqml_global.h"
#include "tgabstractlistmodel.h"

class DialogObject;
class TelegramQml;
class TelegramDialogsModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramDialogsModel : public TgAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(DialogsRoles)

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool initializing READ initializing NOTIFY initializingChanged)
    Q_PROPERTY(bool stopUpdating READ stopUpdating WRITE setStopUpdating NOTIFY stopUpdatingChanged)

public:
    enum DialogsRoles {
        ItemRole = Qt::UserRole,
        SectionRole
    };

    TelegramDialogsModel(QObject *parent = 0);
    ~TelegramDialogsModel();

    TelegramQml *telegram() const;
    void setTelegram(TelegramQml *tg );

    bool stopUpdating() const;
    void setStopUpdating(bool stt);

    qint64 id( const QModelIndex &index ) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;

    int count() const;
    bool initializing() const;

    Q_INVOKABLE int indexOf(DialogObject *dialog);
    Q_INVOKABLE DialogObject *at(int row);

public Q_SLOTS:
    void refreshDatabase();
    void recheck();

Q_SIGNALS:
    void telegramChanged();
    void countChanged();
    void initializingChanged();
    void stopUpdatingChanged();

private Q_SLOTS:
    void dialogsChanged(bool cachedData);
    void dialogsChanged_priv();
    void userDataChanged();

    QList<qint64> fixDialogs(QList<qint64> dialogs );

protected:
    void timerEvent(QTimerEvent *e);

private:
    TelegramDialogsModelPrivate *p;
};

#endif // TELEGRAMDIALOGSMODEL_H
