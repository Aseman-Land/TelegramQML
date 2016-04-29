# MessageListModel

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Methods](#methods)
 * [Roles](#roles)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|<font color='#074885'>MessageListModel</font>|
|C++ class|<font color='#074885'>TelegramMessageListModel</font>|
|Inherits|<font color='#074885'>[AbstractEngineListModel](abstractenginelistmodel.md)</font>|
|Model|<font color='#074885'>Yes</font>|


### Required Properties

* <font color='#074885'><b>engine</b></font>: [Engine](engine.md)
* <font color='#074885'><b>currentPeer</b></font>: [InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md)


### Normal Properties

* <font color='#074885'><b>refreshing</b></font>: boolean (readOnly)
* <font color='#074885'><b>messageList</b></font>: list&lt;int&gt;
* <font color='#074885'><b>dateConvertorMethod</b></font>: function()
* <font color='#074885'><b>key</b></font>: byte (readOnly)
* <font color='#074885'><b>typingUsers</b></font>: list&lt;variant&gt; (readOnly)
* <font color='#074885'><b>limit</b></font>: int


### Methods

 * boolean <font color='#074885'><b>sendMessage</b></font>(string message, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup)


 * boolean <font color='#074885'><b>sendMessage</b></font>(string message, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo)


 * boolean <font color='#074885'><b>sendMessage</b></font>(string message)


 * boolean <font color='#074885'><b>sendFile</b></font>(int type, string file, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup)


 * boolean <font color='#074885'><b>sendFile</b></font>(int type, string file, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo)


 * boolean <font color='#074885'><b>sendFile</b></font>(int type, string file)


 * void <font color='#074885'><b>deleteMessages</b></font>(list&lt;int&gt; msgs)


 * void <font color='#074885'><b>forwardMessages</b></font>([InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md) fromInputPeer, list&lt;int&gt; msgs)


 * void <font color='#074885'><b>resendMessage</b></font>(int msgId, string newCaption)


 * void <font color='#074885'><b>resendMessage</b></font>(int msgId)


 * void <font color='#074885'><b>markAsRead</b></font>()


 * void <font color='#074885'><b>loadFrom</b></font>(int msgId)


 * void <font color='#074885'><b>loadBack</b></font>()


 * void <font color='#074885'><b>loadFront</b></font>()





### Enumerator


##### DataRoles

|Key|Value|
|---|-----|
|RoleMessageItem|256|
|RoleMediaItem|257|
|RoleServiceItem|258|
|RoleMarkupItem|259|
|RoleEntityList|260|
|RoleFromUserItem|261|
|RoleToPeerItem|262|
|RoleMessage|263|
|RoleDateTime|264|
|RoleDate|265|
|RoleUnread|266|
|RoleSent|267|
|RoleOut|268|
|RoleReplyMsgId|269|
|RoleReplyMessage|270|
|RoleReplyPeer|271|
|RoleForwardFromPeer|272|
|RoleForwardDate|273|
|RoleMessageType|274|
|RoleReplyType|275|
|RoleFileName|276|
|RoleFileMimeType|277|
|RoleFileTitle|278|
|RoleFilePerformer|279|
|RoleFileDuration|280|
|RoleFileIsVoice|281|
|RoleFileSize|282|
|RoleDownloadable|283|
|RoleUploading|284|
|RoleDownloading|285|
|RoleTransfaring|286|
|RoleTransfared|287|
|RoleTransfaredSize|288|
|RoleTotalSize|289|
|RoleFilePath|290|
|RoleThumbPath|291|

##### MessageType

|Key|Value|
|---|-----|
|TypeTextMessage|0|
|TypeDocumentMessage|1|
|TypeVideoMessage|2|
|TypeAudioMessage|3|
|TypeVenueMessage|4|
|TypeWebPageMessage|5|
|TypeGeoMessage|6|
|TypeContactMessage|7|
|TypeActionMessage|8|
|TypePhotoMessage|9|
|TypeStickerMessage|10|
|TypeAnimatedMessage|11|
|TypeUnsupportedMessage|12|


### Roles

 * model.<font color='#074885'>item</font>
 * model.<font color='#074885'>chat</font>
 * model.<font color='#074885'>user</font>
 * model.<font color='#074885'>topMessage</font>
 * model.<font color='#074885'>topMessage</font>
 * model.<font color='#074885'>fromUserItem</font>
 * model.<font color='#074885'>toPeerItem</font>
 * model.<font color='#074885'>message</font>
 * model.<font color='#074885'>dateTime</font>
 * model.<font color='#074885'>date</font>
 * model.<font color='#074885'>unread</font>
 * model.<font color='#074885'>sent</font>
 * model.<font color='#074885'>out</font>
 * model.<font color='#074885'>replyMsgId</font>
 * model.<font color='#074885'>replyMessage</font>
 * model.<font color='#074885'>replyPeer</font>
 * model.<font color='#074885'>forwardFromPeer</font>
 * model.<font color='#074885'>forwardDate</font>
 * model.<font color='#074885'>messageType</font>
 * model.<font color='#074885'>replyType</font>
 * model.<font color='#074885'>fileName</font>
 * model.<font color='#074885'>fileMimeType</font>
 * model.<font color='#074885'>fileTitle</font>
 * model.<font color='#074885'>filePerformer</font>
 * model.<font color='#074885'>fileDuration</font>
 * model.<font color='#074885'>fileIsVoice</font>
 * model.<font color='#074885'>fileSize</font>
 * model.<font color='#074885'>downloadable</font>
 * model.<font color='#074885'>uploading</font>
 * model.<font color='#074885'>downloading</font>
 * model.<font color='#074885'>transfaring</font>
 * model.<font color='#074885'>transfared</font>
 * model.<font color='#074885'>transfaredSize</font>
 * model.<font color='#074885'>totalSize</font>
 * model.<font color='#074885'>filePath</font>
 * model.<font color='#074885'>thumbPath</font>
