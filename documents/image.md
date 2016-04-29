# Image

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Methods](#methods)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|Image|
|C++ class|TelegramImageElement|
|Inherits|Item|
|Model|No|


### Required Properties

|Property|Type|Flags|
|--------|----|-----|
|source|[TypeQObject](typeqobject.md)||
|engine|[Engine](engine.md)||


### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|fileSize|int|readonly|
|downloadedSize|int|readonly|
|downloading|boolean|readonly|
|downloaded|boolean|readonly|
|thumbnailDownloaded|boolean|readonly|
|destination|url|readonly|
|thumbnail|url|readonly|
|currentImage|url|readonly|
|imageSize|size|readonly|
|errorText|string|readonly|
|errorCode|int|readonly|
|asynchronous|boolean||
|autoTransform|boolean||
|cache|boolean||
|fillMode|int||
|horizontalAlignment|int||
|mipmap|boolean||
|mirror|boolean||
|paintedHeight|real|readonly|
|paintedWidth|real|readonly|
|smooth|boolean||
|sourceSize|size||
|verticalAlignment|int||



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




