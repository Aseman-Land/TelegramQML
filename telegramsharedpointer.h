#ifndef TELEGRAMSHAREDPOINTER_H
#define TELEGRAMSHAREDPOINTER_H

#include "telegramqml_global.h"

#include <QSharedPointer>
#include <QHash>
#include <QSet>

void TELEGRAMQMLSHARED_EXPORT tg_share_pointer_append(void *dis, void *ptr);
bool TELEGRAMQMLSHARED_EXPORT tg_share_pointer_remove(void *dis, void *ptr);

#ifdef DISABLE_SHARED_POINTER
template<typename T>
using TelegramSharedPointer = T*;
#else
template<class T>
class TELEGRAMQMLSHARED_EXPORT TelegramSharedPointer
{
public:
    TelegramSharedPointer(T *ptr = 0): value(0) { operator =(ptr); }
    virtual ~TelegramSharedPointer() { operator =(0); }

    inline T *data() const { return value; }
    inline bool isNull() const { return !data(); }
    inline bool operator !() const { return isNull(); }
    inline T &operator*() const { return *data(); }
    inline T *operator->() const { return data(); }
    inline operator T*() const { return data(); }
    inline void operator=(const TelegramSharedPointer<T> &b) { operator=(b.value); }
    inline void operator=(T *b) {
        if(value && tg_share_pointer_remove(this, value)) delete value;
        value = b;
        if(value) tg_share_pointer_append(this, value);
    }

private:
    T *value;
};
#endif

#endif // TELEGRAMSHAREDPOINTER_H
