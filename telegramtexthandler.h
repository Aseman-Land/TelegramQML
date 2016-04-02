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

#ifndef TELEGRAMTEXTHANDLER_H
#define TELEGRAMTEXTHANDLER_H

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <QColor>

class UserData;
class TelegramTextHandlerPrivate;
class TelegramTextHandler : public QObject
{
    Q_PROPERTY( QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY( UserData* userData READ userData WRITE setUserData NOTIFY userDataChanged)
    Q_PROPERTY( QVariantMap replacements READ replacements WRITE setReplacements NOTIFY replacementsChanged)
    Q_PROPERTY( bool autoEmojis READ autoEmojis WRITE setAutoEmojis NOTIFY autoEmojisChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged)
    Q_PROPERTY(QColor linkVisitedColor READ linkVisitedColor WRITE setLinkVisitedColor NOTIFY linkVisitedColorChanged)

    Q_OBJECT
public:
    TelegramTextHandler(QObject *parent = 0);
    ~TelegramTextHandler();

    void setCurrentTheme( const QString & theme );
    QString currentTheme() const;

    UserData *userData() const;
    void setUserData(UserData *userData);

    void setReplacements(const QVariantMap &map);
    QVariantMap replacements() const;

    void setLinkColor(const QColor &color);
    QColor linkColor() const;

    void setLinkVisitedColor(const QColor &color);
    QColor linkVisitedColor() const;

    bool autoEmojis() const;
    void setAutoEmojis(bool stt);

    Q_INVOKABLE QString convertSmiliesToEmoji(const QString &text);

    Q_INVOKABLE QString textToEmojiText(const QString & txt , int size = 16, bool skipLinks = false, bool localLinks = false);
    Q_INVOKABLE QString bodyTextToEmojiText( const QString & txt );

    Q_INVOKABLE QList<QString> keys() const;
    Q_INVOKABLE QString pathOf( const QString & key ) const;

    Q_INVOKABLE bool contains(const QString &key) const;

    const QHash<QString,QString> &emojis() const;

Q_SIGNALS:
    void currentThemeChanged();
    void userDataChanged();
    void replacementsChanged();
    void autoEmojisChanged();
    void linkColorChanged();
    void linkVisitedColorChanged();

private:
    TelegramTextHandlerPrivate *p;
};

#endif // TELEGRAMTEXTHANDLER_H
