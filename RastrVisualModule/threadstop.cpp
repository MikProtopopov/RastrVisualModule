#include "threadstop.h"
#include <QMutex>

ThreadStop::ThreadStop()
{
    QMutex *_mutex;
    bool _value;

    value = _value;
    mutex = _mutex;
}

void ThreadStop::markThread2Stop()
{
    stopThread = true;
}

void set(T _value)
{
    QMutexLocker locker(mutex);
    value = _value;
}

bool get()
{
    QMutexLocker locker(mutex);
    return value;
}
