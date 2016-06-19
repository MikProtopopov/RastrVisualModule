#include "threadstop.h"

ThreadStop::ThreadStop(QMutex *_mutex, bool _value)
{
    value = _value;
    mutex = _mutex;
}

void ThreadStop::set(bool _value)
{
    QMutexLocker locker(mutex);
    value = _value;
}

bool ThreadStop::get()
{
    QMutexLocker locker(mutex);
    return value;
}
