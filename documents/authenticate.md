# Authenticate

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Methods](#methods)
 * [Signals](#signals)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|Authenticate|
|C++ class|TelegramAuthenticate|
|Inherits|Tq|
|Model|No|


### Required Properties

|Property|Type|Flags|
|--------|----|-----|
|engine|[Engine](engine.md)||


### Normal Properties

|Property|Type|Flags|
|--------|----|-----|
|state|int|readonly|
|callTimeout|int|readonly|
|remainingTime|int|readonly|


### Enumerator


##### AuthState

|Key|Value|
|---|-----|
|AuthUnknown|0|
|AuthCheckingPhone|1|
|AuthCheckingPhoneError|2|
|AuthSignUpNeeded|3|
|AuthCodeRequesting|4|
|AuthCodeRequestingError|5|
|AuthCodeRquested|6|
|AuthLoggingIn|7|
|AuthLoggingInError|8|
|AuthLoggedIn|9|


### Methods

```c++
void signUp(string firstName, string lastName)
```

```c++
void signIn(string code)
```



### Signals

```c++
void authLoggedIn()
```



### Enumerator


##### AuthState

|Key|Value|
|---|-----|
|AuthUnknown|0|
|AuthCheckingPhone|1|
|AuthCheckingPhoneError|2|
|AuthSignUpNeeded|3|
|AuthCodeRequesting|4|
|AuthCodeRequestingError|5|
|AuthCodeRquested|6|
|AuthLoggingIn|7|
|AuthLoggingInError|8|
|AuthLoggedIn|9|

