# DialogListModel

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Roles](#roles)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|DialogListModel|
|C++ class|TelegramDialogListModel|
|Inherits|[AbstractEngineListModel](abstractenginelistmodel.md)|
|Model|Yes|


### Required Properties

|Property|Type|Flags|
|--------|----|-----|
|engine|[Engine](engine.md)||


### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|visibility|int||
|sortFlag|list&lt;int&gt;||
|dateConvertorMethod|function()||
|refreshing|boolean|readonly|
|categories|QVariantMap||


### Enumerator


##### VisibilityFlags

|Key|Value|
|---|-----|
|VisibilityEmptyDialogs|1|
|VisibilityDeletedDialogs|2|
|VisibilityOnlineUsersOnly|4|
|VisibilityContacts|8|
|VisibilityNonContacts|16|
|VisibilityBots|32|
|VisibilityUsers|24|
|VisibilityChats|64|
|VisibilityChannels|128|
|VisibilitySecretChats|256|
|VisibilityAll|507|

##### SortFlag

|Key|Value|
|---|-----|
|SortByType|0|
|SortByName|1|
|SortByDate|2|
|SortByUnreads|3|
|SortByOnline|4|
|SortByCategories|5|

##### DataRoles

|Key|Value|
|---|-----|
|RoleDialogItem|256|
|RoleChatItem|257|
|RoleUserItem|258|
|RoleTopMessageItem|259|
|RolePeerItem|260|
|RolePeerHex|261|
|RoleName|262|
|RoleMessageDate|263|
|RoleMessageUnread|264|
|RoleMessage|265|
|RoleMessageOut|266|
|RoleLastOnline|267|
|RoleIsOnline|268|
|RoleStatus|269|
|RoleStatusText|270|
|RoleTyping|271|
|RoleUnreadCount|272|
|RoleMute|273|
|RoleCategory|274|




### Enumerator


##### VisibilityFlags

|Key|Value|
|---|-----|
|VisibilityEmptyDialogs|1|
|VisibilityDeletedDialogs|2|
|VisibilityOnlineUsersOnly|4|
|VisibilityContacts|8|
|VisibilityNonContacts|16|
|VisibilityBots|32|
|VisibilityUsers|24|
|VisibilityChats|64|
|VisibilityChannels|128|
|VisibilitySecretChats|256|
|VisibilityAll|507|

##### SortFlag

|Key|Value|
|---|-----|
|SortByType|0|
|SortByName|1|
|SortByDate|2|
|SortByUnreads|3|
|SortByOnline|4|
|SortByCategories|5|

##### DataRoles

|Key|Value|
|---|-----|
|RoleDialogItem|256|
|RoleChatItem|257|
|RoleUserItem|258|
|RoleTopMessageItem|259|
|RolePeerItem|260|
|RolePeerHex|261|
|RoleName|262|
|RoleMessageDate|263|
|RoleMessageUnread|264|
|RoleMessage|265|
|RoleMessageOut|266|
|RoleLastOnline|267|
|RoleIsOnline|268|
|RoleStatus|269|
|RoleStatusText|270|
|RoleTyping|271|
|RoleUnreadCount|272|
|RoleMute|273|
|RoleCategory|274|


### Roles

* model.dialog
* model.chat
* model.user
* model.topMessage
* model.peer
* model.peerHex
* model.title
* model.messageDate
* model.messageUnread
* model.message
* model.messageOut
* model.lastOnline
* model.isOnline
* model.status
* model.statusText
* model.typing
* model.unreadCount
* model.mute
* model.category
