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
|Component|MessageListModel|
|C++ class|TelegramMessageListModel|
|Inherits|[AbstractEngineListModel](abstractenginelistmodel.md)|
|Model|Yes|


### Required Properties

|Property|Type|Flags|
|--------|----|-----|
|engine|[Engine](engine.md)||
|currentPeer|InputPeer||


### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|refreshing|boolean|readonly|
|messageList|list&lt;int&gt;||
|dateConvertorMethod|function()||
|key|byte|readonly|
|typingUsers|list&lt;variant&gt;|readonly|
|limit|int||


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


### Methods

```c++
boolean sendMessage(string message, Message replyTo, ReplyMarkup replyMarkup)
```

```c++
boolean sendMessage(string message, Message replyTo)
```

```c++
boolean sendMessage(string message)
```

```c++
boolean sendFile(int type, string file, Message replyTo, ReplyMarkup replyMarkup)
```

```c++
boolean sendFile(int type, string file, Message replyTo)
```

```c++
boolean sendFile(int type, string file)
```

```c++
void deleteMessages(list&lt;int&gt; msgs)
```

```c++
void forwardMessages(InputPeer fromInputPeer, list&lt;int&gt; msgs)
```

```c++
void resendMessage(int msgId, string newCaption)
```

```c++
void resendMessage(int msgId)
```

```c++
void markAsRead()
```

```c++
void loadFrom(int msgId)
```

```c++
void loadBack()
```

```c++
void loadFront()
```




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

* model.item
* model.chat
* model.user
* model.topMessage
* model.topMessage
* model.fromUserItem
* model.toPeerItem
* model.message
* model.dateTime
* model.date
* model.unread
* model.sent
* model.out
* model.replyMsgId
* model.replyMessage
* model.replyPeer
* model.forwardFromPeer
* model.forwardDate
* model.messageType
* model.replyType
* model.fileName
* model.fileMimeType
* model.fileTitle
* model.filePerformer
* model.fileDuration
* model.fileIsVoice
* model.fileSize
* model.downloadable
* model.uploading
* model.downloading
* model.transfaring
* model.transfared
* model.transfaredSize
* model.totalSize
* model.filePath
* model.thumbPath
