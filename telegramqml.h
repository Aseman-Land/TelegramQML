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

#ifndef TELEGRAMQML_H
#define TELEGRAMQML_H

#include <QObject>
#include <QStringList>
#include "types/inputfilelocation.h"
#include "types/peer.h"
#include "types/inputpeer.h"

#include "telegramqml_global.h"

class TelegramSearchModel;
class UpdatesState;
class NewsLetterDialog;
class DownloadObject;
class Database;
class SecretChat;
class EncryptedFile;
class EncryptedFileObject;
class DecryptedMessage;
class DecryptedMessageObject;
class PeerNotifySettings;
class EncryptedChat;
class EncryptedChatObject;
class EncryptedMessage;
class EncryptedMessageObject;
class DocumentObject;
class VideoObject;
class SecretChatMessage;
class AudioObject;
class WallPaper;
class WallPaperObject;
class UserData;
class StorageFileType;
class FileLocationObject;
class PhotoObject;
class ContactsLink;
class Update;
class Message;
class AccountPassword;
class AffectedMessages;
class ImportedContact;
class TelegramMessagesModel;
class User;
class Contact;
class ContactObject;
class Chat;
class ChatFull;
class Dialog;
class Photo;
class UserProfilePhoto;
class DialogObject;
class MessageObject;
class ContactFound;
class InputPeerObject;
class ChatFullObject;
class ChatObject;
class UserObject;
class UploadObject;
class Telegram;
class TelegramQmlPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramQml : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString defaultHostAddress READ defaultHostAddress WRITE setDefaultHostAddress NOTIFY defaultHostAddressChanged)
    Q_PROPERTY(int defaultHostPort READ defaultHostPort WRITE setDefaultHostPort NOTIFY defaultHostPortChanged)
    Q_PROPERTY(int defaultHostDcId READ defaultHostDcId WRITE setDefaultHostDcId NOTIFY defaultHostDcIdChanged)
    Q_PROPERTY(int appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(QString appHash READ appHash WRITE setAppHash NOTIFY appHashChanged)

    Q_PROPERTY(QString phoneNumber   READ phoneNumber   WRITE setPhoneNumber   NOTIFY phoneNumberChanged  )
    Q_PROPERTY(QString configPath    READ configPath    WRITE setConfigPath    NOTIFY configPathChanged   )
    Q_PROPERTY(QString publicKeyFile READ publicKeyFile WRITE setPublicKeyFile NOTIFY publicKeyFileChanged)
    Q_PROPERTY(QString downloadPath  READ downloadPath  WRITE setDownloadPath  NOTIFY downloadPathChanged )
    Q_PROPERTY(QString tempPath      READ tempPath      WRITE setTempPath      NOTIFY tempPathChanged     )

    Q_PROPERTY(QObject* newsLetterDialog READ newsLetterDialog WRITE setNewsLetterDialog NOTIFY newsLetterDialogChanged     )
    Q_PROPERTY(bool autoCleanUpMessages READ autoCleanUpMessages WRITE setAutoCleanUpMessages NOTIFY autoCleanUpMessagesChanged)

    Q_PROPERTY(bool  online               READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(int   unreadCount          READ unreadCount            NOTIFY unreadCountChanged)
    Q_PROPERTY(qreal totalUploadedPercent READ totalUploadedPercent   NOTIFY totalUploadedPercentChanged)

    Q_PROPERTY(bool uploadingProfilePhoto READ uploadingProfilePhoto NOTIFY uploadingProfilePhotoChanged)

    Q_PROPERTY(Telegram* telegram READ telegram NOTIFY telegramChanged)
    Q_PROPERTY(UserData* userData READ userData NOTIFY userDataChanged)
    Q_PROPERTY(qint64    me       READ me       NOTIFY meChanged)
    Q_PROPERTY(qint64    cutegramId READ cutegramId NOTIFY fakeSignal)

    Q_PROPERTY(bool authNeeded          READ authNeeded          NOTIFY authNeededChanged         )
    Q_PROPERTY(bool authLoggedIn        READ authLoggedIn        NOTIFY authLoggedInChanged       )
    Q_PROPERTY(bool authPhoneRegistered READ authPhoneRegistered NOTIFY authPhoneRegisteredChanged)
    Q_PROPERTY(bool authPhoneInvited    READ authPhoneInvited    NOTIFY authPhoneInvitedChanged   )
    Q_PROPERTY(bool authPhoneChecked    READ authPhoneChecked    NOTIFY authPhoneCheckedChanged   )
    Q_PROPERTY(bool connected           READ connected           NOTIFY connectedChanged          )

    Q_PROPERTY(QString authSignUpError READ authSignUpError NOTIFY authSignUpErrorChanged)
    Q_PROPERTY(QString authSignInError READ authSignInError NOTIFY authSignInErrorChanged)
    Q_PROPERTY(QString error           READ error           NOTIFY errorChanged          )

    Q_PROPERTY(DialogObject* nullDialog READ nullDialog NOTIFY fakeSignal)
    Q_PROPERTY(MessageObject* nullMessage READ nullMessage NOTIFY fakeSignal)
    Q_PROPERTY(ChatObject* nullChat READ nullChat NOTIFY fakeSignal)
    Q_PROPERTY(UserObject* nullUser READ nullUser NOTIFY fakeSignal)
    Q_PROPERTY(WallPaperObject* nullWallpaper READ nullWallpaper NOTIFY fakeSignal)
    Q_PROPERTY(UploadObject* nullUpload READ nullUpload NOTIFY fakeSignal)
    Q_PROPERTY(ChatFullObject* nullChatFull READ nullChatFull NOTIFY fakeSignal)
    Q_PROPERTY(ContactObject* nullContact READ nullContact NOTIFY fakeSignal)
    Q_PROPERTY(FileLocationObject* nullLocation READ nullLocation NOTIFY fakeSignal)
    Q_PROPERTY(EncryptedChatObject* nullEncryptedChat READ nullEncryptedChat NOTIFY fakeSignal)
    Q_PROPERTY(EncryptedMessageObject* nullEncryptedMessage READ nullEncryptedMessage NOTIFY fakeSignal)

public:
    TelegramQml(QObject *parent = 0);
    ~TelegramQml();

    QString phoneNumber() const;
    void setPhoneNumber( const QString & phone );

    QString downloadPath() const;
    void setDownloadPath( const QString & downloadPath );

    QString tempPath() const;
    void setTempPath( const QString & tempPath );

    QString configPath() const;
    void setConfigPath( const QString & conf );

    QString publicKeyFile() const;
    void setPublicKeyFile( const QString & file );

    void setDefaultHostAddress(const QString &host);
    QString defaultHostAddress() const;

    void setDefaultHostPort(int port);
    int defaultHostPort() const;

    void setDefaultHostDcId(int dcId);
    int defaultHostDcId() const;

    void setAppId(int appId);
    int appId() const;

    void setAppHash(const QString &appHash);
    QString appHash() const;

    void setNewsLetterDialog(QObject *dialog);
    QObject *newsLetterDialog() const;

    void setAutoCleanUpMessages(bool stt);
    bool autoCleanUpMessages() const;

    void registerMessagesModel(TelegramMessagesModel *model);
    void unregisterMessagesModel(TelegramMessagesModel *model);

    void registerSearchModel(TelegramSearchModel *model);
    void unregisterSearchModel(TelegramSearchModel *model);

    UserData *userData() const;
    Database *database() const;
    Telegram *telegram() const;
    qint64 me() const;
    qint64 cutegramId() const;

    bool online() const;
    void setOnline( bool stt );

    void setInvisible( bool stt );
    bool invisible() const;

    int unreadCount() const;
    qreal totalUploadedPercent() const;

    bool authNeeded() const;
    bool authLoggedIn() const;
    bool authPhoneChecked() const;
    bool authPhoneRegistered() const;
    bool authPhoneInvited() const;
    bool connected() const;

    bool uploadingProfilePhoto() const;

    QString authSignUpError() const;
    QString authSignInError() const;
    QString error() const;

    Q_INVOKABLE DialogObject *dialog(qint64 id) const;
    Q_INVOKABLE MessageObject *message(qint64 id) const;
    Q_INVOKABLE ChatObject *chat(qint64 id) const;
    Q_INVOKABLE UserObject *user(qint64 id) const;
    Q_INVOKABLE qint64 messageDialogId(qint64 id) const;
    Q_INVOKABLE DialogObject *messageDialog(qint64 id) const;
    Q_INVOKABLE WallPaperObject *wallpaper(qint64 id) const;
    Q_INVOKABLE MessageObject *upload(qint64 id) const;
    Q_INVOKABLE ChatFullObject *chatFull(qint64 id) const;
    Q_INVOKABLE ContactObject *contact(qint64 id) const;
    Q_INVOKABLE EncryptedChatObject *encryptedChat(qint64 id) const;

    Q_INVOKABLE FileLocationObject *locationOf(qint64 id, qint64 dcId, qint64 accessHash, QObject *parent);
    Q_INVOKABLE FileLocationObject *locationOfDocument(DocumentObject *doc);
    Q_INVOKABLE FileLocationObject *locationOfVideo(VideoObject *vid);
    Q_INVOKABLE FileLocationObject *locationOfAudio(AudioObject *aud);

    Q_INVOKABLE bool documentIsSticker(DocumentObject *doc);
    Q_INVOKABLE QString documentFileName(DocumentObject *doc);

    Q_INVOKABLE DialogObject *fakeDialogObject( qint64 id, bool isChat );

    DialogObject *nullDialog() const;
    MessageObject *nullMessage() const;
    ChatObject *nullChat() const;
    UserObject *nullUser() const;
    WallPaperObject *nullWallpaper() const;
    UploadObject *nullUpload() const;
    ChatFullObject *nullChatFull() const;
    ContactObject *nullContact() const;
    FileLocationObject *nullLocation() const;
    EncryptedChatObject *nullEncryptedChat() const;
    EncryptedMessageObject *nullEncryptedMessage() const;

    Q_INVOKABLE QString fileLocation( FileLocationObject *location );
    Q_INVOKABLE QString videoThumbLocation( const QString &path );
    Q_INVOKABLE QString audioThumbLocation( const QString &path );

    QList<qint64> dialogs() const;
    QList<qint64> messages(qint64 did, qint64 maxId = 0) const;
    QList<qint64> wallpapers() const;
    QList<qint64> uploads() const;
    QList<qint64> contacts() const;

    InputPeer getInputPeer(qint64 pid);

    QList<qint64> userIndex(const QString &keyword);

public Q_SLOTS:
    void authLogout();
    void authSendCall();
    void authSendCode();
    void authSendInvites(const QStringList &phoneNumbers, const QString &inviteText);
    void authSignIn(const QString &code);
    void authSignUp(const QString &code, const QString &firstName, const QString &lastName);

    void sendMessage( qint64 dialogId, const QString & msg, int replyTo = 0 );
    bool sendMessageAsDocument( qint64 dialogId, const QString & msg );
    void sendGeo(qint64 dialogId, qreal latitude, qreal longitude, int replyTo = 0);

    void addContact(const QString &firstName, const QString &lastName, const QString &phoneNumber);

    void forwardMessages( QList<int> msgIds, qint64 peerId );
    void deleteMessages(QList<int> msgIds );

    void deleteCutegramDialog();
    void messagesCreateChat(const QList<int> &users, const QString & topic );
    void messagesAddChatUser(qint64 chatId, qint64 userId, qint32 fwdLimit = 0);
    void messagesDeleteChatUser(qint64 chatId, qint64 userId);
    void messagesEditChatTitle(qint32 chatId, const QString &title);

    void messagesDeleteHistory(qint64 peerId);
    void messagesSetTyping(qint64 peerId, bool stt);
    void messagesReadHistory(qint64 peerId);

    void messagesCreateEncryptedChat(qint64 userId);
    void messagesAcceptEncryptedChat(qint32 chatId);
    void messagesDiscardEncryptedChat(qint32 chatId);

    void messagesGetFullChat(qint32 chatId);

    void search(const QString &keyword);
    void searchContact(const QString &keyword);

    bool sendFile(qint64 dialogId, const QString & file , bool forceDocument = false, bool forceAudio = false);
    void getFile(FileLocationObject *location, qint64 type = InputFileLocation::typeInputFileLocation , qint32 fileSize = 0);
    void getFileJustCheck(FileLocationObject *location);
    void cancelDownload(DownloadObject *download);
    void cancelSendGet( qint64 fileId );

    void setProfilePhoto( const QString & fileName );

    void timerUpdateDialogs( qint32 duration = 1000 );
    void cleanUpMessages();

    void updatesGetState();
    void updatesGetDifference();

Q_SIGNALS:
    void defaultHostAddressChanged();
    void defaultHostPortChanged();
    void defaultHostDcIdChanged();
    void appIdChanged();
    void appHashChanged();

    void phoneNumberChanged();
    void configPathChanged();
    void publicKeyFileChanged();
    void telegramChanged();
    void autoCleanUpMessagesChanged();
    void userDataChanged();
    void onlineChanged();
    void downloadPathChanged();
    void tempPathChanged();
    void dialogsChanged(bool cachedData);
    void messagesChanged(bool cachedData);
    void wallpapersChanged();
    void uploadsChanged();
    void chatFullsChanged();
    void contactsChanged();
    void autoUpdateChanged();
    void encryptedChatsChanged();
    void uploadingProfilePhotoChanged();
    void newsLetterDialogChanged();

    void unreadCountChanged();
    void totalUploadedPercentChanged();
    void invisibleChanged();

    void authNeededChanged();
    void authLoggedInChanged();
    void authPhoneRegisteredChanged();
    void authPhoneInvitedChanged();
    void authPhoneCheckedChanged();
    void authPasswordProtectedError();
    void connectedChanged();

    void authSignUpErrorChanged();
    void authSignInErrorChanged();

    void authCodeRequested( bool phoneRegistered, qint32 sendCallTimeout );
    void authCallRequested( bool ok );
    void authInvitesSent( bool ok );

    void userBecomeOnline(qint64 userId);
    void userStartTyping(qint64 userId, qint64 dId);

    void errorChanged();
    void meChanged();
    void fakeSignal();

    void incomingMessage( MessageObject *msg );
    void incomingEncryptedMessage( EncryptedMessageObject *msg );

    void searchDone(const QList<qint64> &messages);
    void contactsFounded(const QList<qint32> &contacts);

protected:
    void try_init();

private Q_SLOTS:
    void authNeeded_slt();
    void authLoggedIn_slt();
    void authLogOut_slt(qint64 id, bool ok);
    void authSendCode_slt(qint64 id, bool phoneRegistered, qint32 sendCallTimeout);
    void authSendCall_slt(qint64 id, bool ok);
    void authSendInvites_slt(qint64 id, bool ok);
    void authCheckPassword_slt(qint64 msgId, qint32 expires, const User &user);
    void authCheckPhone_slt(qint64 id, bool phoneRegistered);
    void authSignInError_slt(qint64 id, qint32 errorCode, QString errorText);
    void authSignUpError_slt(qint64 id, qint32 errorCode, QString errorText);
    void error(qint64 id, qint32 errorCode, QString functionName, QString errorText);

    void accountGetPassword_slt(qint64 msgId, const AccountPassword &password);
    void accountGetWallPapers_slt(qint64 id, const QList<WallPaper> & wallPapers);
    void photosUploadProfilePhoto_slt(qint64 id, const Photo & photo, const QList<User> & users);
    void photosUpdateProfilePhoto_slt(qint64 id, const UserProfilePhoto & userProfilePhoto);
    void contactsImportContacts_slt(qint64 id, const QList<ImportedContact> &importedContacts, const QList<qint64> &retryContacts, const QList<User> &users);
    void contactsFound_slt(qint64 id, const QList<ContactFound> &founds, const QList<User> &users);

    void contactsGetContacts_slt(qint64 id, bool modified, const QList<Contact> & contacts, const QList<User> & users);
    void usersGetFullUser_slt(qint64 id, const User &user, const ContactsLink &link, const Photo &profilePhoto, const PeerNotifySettings &notifySettings, bool blocked, const QString &realFirstName, const QString &realLastName);

    void messagesSendMessage_slt(qint64 id, qint32 msgId, qint32 date, qint32 pts, qint32 pts_count, qint32 seq, const QList<ContactsLink> & links);
    void messagesForwardMessage_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesForwardMessages_slt(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesDeleteMessages_slt(qint64 id, const AffectedMessages &deletedMessages);
    void messagesGetMessages_slt(qint64 id, qint32 sliceCount, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);

    void messagesSendMedia_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 seq);
    void messagesSendPhoto_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 seq);
    void messagesSendVideo_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 seq);
    void messagesSendAudio_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 seq);
    void messagesSendDocument_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 seq);
    void messagesGetDialogs_slt(qint64 id, qint32 sliceCount, const QList<Dialog> & dialogs, const QList<Message> & messages, const QList<Chat> & chats, const QList<User> & users);
    void messagesGetHistory_slt(qint64 id, qint32 sliceCount, const QList<Message> & messages, const QList<Chat> & chats, const QList<User> & users);
    void messagesDeleteHistory_slt(qint64 id, qint32 pts, qint32 seq, qint32 offset);

    void messagesSearch_slt(qint64 id, qint32 sliceCount, const QList<Message> & messages, const QList<Chat> & chats, const QList<User> & users);

    void messagesGetFullChat_slt(qint64 id, const ChatFull & chatFull, const QList<Chat> & chats, const QList<User> & users);
    void messagesCreateChat_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 ptsCount, qint32 seq);
    void messagesEditChatTitle_slt(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount, qint32 seq);
    void messagesEditChatPhoto_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 ptsCount, qint32 seq);
    void messagesAddChatUser_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 ptsCount, qint32 seq);
    void messagesDeleteChatUser_slt(qint64 id, const Message & message, const QList<Chat> & chats, const QList<User> & users, const QList<ContactsLink> & links, qint32 pts, qint32 ptsCount, qint32 seq);

    void messagesCreateEncryptedChat_slt(qint32 chatId, qint32 date, qint32 peerId, qint64 accessHash);
    void messagesEncryptedChatRequested_slt(qint32 chatId, qint32 date, qint32 creatorId, qint64 creatorAccessHash);
    void messagesEncryptedChatCreated_slt(qint32 chatId);
    void messagesEncryptedChatDiscarded_slt(qint32 chatId);
    void messagesSendEncrypted_slt(qint64 id, qint32 date, const EncryptedFile &encryptedFile);
    void messagesSendEncryptedFile_slt(qint64 id, qint32 date, const EncryptedFile &encryptedFile);

    void updatesTooLong_slt();
    void updateShortMessage_slt(qint32 id, qint32 userId, QString message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShortChatMessage_slt(qint32 id, qint32 fromId, qint32 chatId, QString message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShort_slt(const Update & update, qint32 date);
    void updatesCombined_slt(const QList<Update> & updates, const QList<User> & users, const QList<Chat> & chats, qint32 date, qint32 seqStart, qint32 seq);
    void updates_slt(const QList<Update> & udts, const QList<User> & users, const QList<Chat> & chats, qint32 date, qint32 seq);
    void updateSecretChatMessage_slt(const SecretChatMessage &secretChatMessage, qint32 qts);
    void updatesGetDifference_slt(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);
    void updatesGetState_slt(qint64 id, qint32 pts, qint32 qts, qint32 date, qint32 seq, qint32 unreadCount);

    void uploadGetFile_slt(qint64 id, const StorageFileType & type, qint32 mtime, const QByteArray & bytes, qint32 partId, qint32 downloaded, qint32 total);
    void uploadSendFile_slt(qint64 fileId, qint32 partId, qint32 uploaded, qint32 totalSize);
    void uploadCancelFile_slt(qint64 fileId, bool cancelled);

    void incomingAsemanMessage(const Message &msg, const Dialog &dialog);

private:
    void insertDialog(const Dialog & dialog , bool encrypted = false, bool fromDb = false);
    void insertMessage(const Message & message , bool encrypted = false, bool fromDb = false, bool tempMsg = false);
    void insertUser( const User & user, bool fromDb = false );
    void insertChat( const Chat & chat, bool fromDb = false );
    void insertUpdate( const Update & update );
    void insertContact( const Contact & contact );
    void insertEncryptedMessage(const EncryptedMessage & emsg);
    void insertEncryptedChat(const EncryptedChat & c);
    void insertSecretChatMessage(const SecretChatMessage & sc, bool cachedMsg = false);

    QString fileLocation_old( FileLocationObject *location );
    QString fileLocation_old2( FileLocationObject *location );

    static QString localFilesPrePath();
    static bool createVideoThumbnail(const QString &video, const QString &output, QString ffmpegPath = QString());
    static bool createAudioThumbnail(const QString &audio, const QString &output);

protected:
    void timerEvent(QTimerEvent *e);
    Message newMessage(qint64 dId);
    SecretChat *getSecretChat(qint64 chatId);

    void startGarbageChecker();
    void insertToGarbeges(QObject *obj);

private Q_SLOTS:
    void dbUserFounded(const User &user);
    void dbChatFounded(const Chat &chat);
    void dbDialogFounded(const Dialog &dialog, bool encrypted);
    void dbMessageFounded(const Message &message);
    void dbMediaKeysFounded(qint64 mediaId, const QByteArray &key, const QByteArray &iv);

    void refreshUnreadCount();
    void refreshTotalUploadedPercent();
    void refreshSecretChats();
    void updateEncryptedTopMessage(const Message &message);

    qint64 generateRandomId() const;
    InputPeer::InputPeerType getInputPeerType(qint64 pid);
    Peer::PeerType getPeerType(qint64 pid);

    QStringList stringToIndex(const QString & str);

    void objectDestroyed(QObject *obj);
    void cleanUpMessages_prv();

    bool requestReadMessage(qint32 msgId);
    void requestReadMessage_prv();

    static void removeFiles(const QString &dir);

private:
    TelegramQmlPrivate *p;
};

Q_DECLARE_METATYPE(TelegramQml*)

#endif // TELEGRAMQML_H
