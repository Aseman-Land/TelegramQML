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
#include <QUrl>

#include "telegramqml_global.h"

class UserData;
class TelegramTextHandlerPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramTextHandler : public QObject
{
    Q_PROPERTY(QUrl emojisSource READ emojisSource WRITE setEmojisSource NOTIFY emojisSourceChanged)
    Q_PROPERTY(QVariantMap replacements READ replacements WRITE setReplacements NOTIFY replacementsChanged)
    Q_PROPERTY(bool autoEmojis READ autoEmojis WRITE setAutoEmojis NOTIFY autoEmojisChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged)
    Q_PROPERTY(QColor linkVisitedColor READ linkVisitedColor WRITE setLinkVisitedColor NOTIFY linkVisitedColorChanged)

    Q_OBJECT
public:
    TelegramTextHandler(QObject *parent = 0);
    ~TelegramTextHandler();

    void setEmojisSource(const QUrl &emojisSource);
    QUrl emojisSource() const;

    void setReplacements(const QVariantMap &map);
    QVariantMap replacements() const;

    void setLinkColor(const QColor &color);
    QColor linkColor() const;

    void setLinkVisitedColor(const QColor &color);
    QColor linkVisitedColor() const;

    bool autoEmojis() const;
    void setAutoEmojis(bool stt);

    static Qt::LayoutDirection directionOf( const QString & str );
    const QHash<QString,QString> &emojis() const;

public Q_SLOTS:
    QString convertSmiliesToEmoji(const QString &text);

    QString textToEmojiText(const QString & txt , int size = 16, bool skipLinks = false);
    QString bodyTextToEmojiText( const QString & txt );

    QList<QString> keys() const;
    QString pathOf( const QString & key ) const;

    bool contains(const QString &key) const;

Q_SIGNALS:
    void emojisSourceChanged();
    void replacementsChanged();
    void autoEmojisChanged();
    void linkColorChanged();
    void linkVisitedColorChanged();

protected:
    void refresh();
    QString emojiPath() const;

private:
    TelegramTextHandlerPrivate *p;
};

#endif // TELEGRAMTEXTHANDLER_H
