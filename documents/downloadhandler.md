# DownloadHandler

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Methods](#methods)
 * [Signals](#signals)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|DownloadHandler|
|C++ class|TelegramDownloadHandler|
|Inherits|Tq|
|Model|No|


### Required Properties

|Property|Type|Flags|
|--------|----|-----|
|source|[TypeQObject](typeqobject.md)||


### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|target|[FileLocation](filelocation.md)|readonly|
|targetType|int|readonly|
|size|int|readonly|
|downloadedSize|int|readonly|
|downloadTotal|int|readonly|
|downloading|boolean|readonly|
|destination|string|readonly|
|thumbnail|string|readonly|
|imageSize|size|readonly|
|thumbnailSize|size|readonly|



### Methods

```c++
boolean download()
```

```c++
boolean check()
```

```c++
void stop()
```



### Signals

```c++
void engineChanged()
```



