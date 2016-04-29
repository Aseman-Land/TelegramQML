# Engine

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Signals](#signals)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|Engine|
|C++ class|TelegramEngine|
|Inherits|Tq|
|Model|No|


### Required Properties

|Property|Type|Flags|
|--------|----|-----|
|phoneNumber|string||
|configDirectory|string||
|app|[App](app.md)||
|host|[Host](host.md)||


### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|profileManager|[ProfileManagerModel](profilemanagermodel.md)||
|our|UserFull|readonly|
|timeout|int||
|state|int|readonly|
|logLevel|int||
|tempPath|string||


### Enumerator


##### AuthState

|Key|Value|
|---|-----|
|AuthUnknown|0|
|AuthInitializing|1|
|AuthNeeded|2|
|AuthFetchingOurDetails|3|
|AuthLoggedIn|4|

##### LogLevel

|Key|Value|
|---|-----|
|LogLevelClean|0|
|LogLevelUseful|1|
|LogLevelFull|2|



### Signals

```c++
void authLoggedIn()
```

```c++
void authNeeded()
```

```c++
void telegramChanged()
```



### Enumerator


##### AuthState

|Key|Value|
|---|-----|
|AuthUnknown|0|
|AuthInitializing|1|
|AuthNeeded|2|
|AuthFetchingOurDetails|3|
|AuthLoggedIn|4|

##### LogLevel

|Key|Value|
|---|-----|
|LogLevelClean|0|
|LogLevelUseful|1|
|LogLevelFull|2|

