# AbstractListModel

 * [Component details](#component-details)
 * [Normal Properties](#normal-properties)
 * [Methods](#methods)
 * [Roles](#roles)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|AbstractListModel|
|C++ class|TelegramAbstractListModel|
|Inherits|QAbstractListModel|
|Model|Yes|



### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|count|int|readonly|
|errorText|string|readonly|
|errorCode|int|readonly|
|items|QQmlListProperty<Q>|readonly|



### Methods

```c++
variant get(int index, int role)
```

```c++
variant get(int index, string roleName)
```

```c++
QVariantMap get(int index)
```




