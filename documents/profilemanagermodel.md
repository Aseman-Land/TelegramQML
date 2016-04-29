# ProfileManagerModel

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
|Component|ProfileManagerModel|
|C++ class|TelegramProfileManagerModel|
|Inherits|[AbstractListModel](abstractlistmodel.md)|
|Model|Yes|


### Required Properties

|Property|Type|Flags|
|--------|----|-----|
|source|string||


### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|engineDelegate|Component||
|initializing|boolean|readonly|


### Enumerator


##### DataRole

|Key|Value|
|---|-----|
|DataPhoneNumber|256|
|DataMute|257|
|DataEngine|258|

##### AddResult

|Key|Value|
|---|-----|
|AddResultSucessfully|0|
|AddResultExists|1|
|AddResultInvalidPhone|2|
|AddResultDatabaseError|3|


### Methods

```c++
void addNew()
```

```c++
int add(string phoneNumber, boolean mute, [Engine](engine.md) engine)
```

```c++
boolean remove(string phoneNumber)
```




### Enumerator


##### DataRole

|Key|Value|
|---|-----|
|DataPhoneNumber|256|
|DataMute|257|
|DataEngine|258|

##### AddResult

|Key|Value|
|---|-----|
|AddResultSucessfully|0|
|AddResultExists|1|
|AddResultInvalidPhone|2|
|AddResultDatabaseError|3|


### Roles

* model.phoneNumber
* model.mute
* model.engine
