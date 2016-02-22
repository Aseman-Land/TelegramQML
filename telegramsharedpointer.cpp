#include "telegramsharedpointer.h"

#include <QHash>
#include <QSet>

#ifndef DISABLE_SHARED_POINTER
QHash<void*, QSet<void*> > tg_share_pointer_data;

void tg_share_pointer_append(void *dis, void *ptr)
{
    tg_share_pointer_data[ptr].insert(dis);
}

bool tg_share_pointer_remove(void *dis, void *ptr)
{
    tg_share_pointer_data[ptr].remove(dis);
    return tg_share_pointer_data[ptr].isEmpty();
}
#endif
