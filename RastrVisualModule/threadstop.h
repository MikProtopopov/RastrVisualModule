#ifndef THREADSTOP_H
#define THREADSTOP_H

#include <QObject>
#include <QMutex>

class ThreadStop : public QObject
{
public:
    ThreadStop(QMutex *_mutex, bool _value);
    QMutex *mutex;
    void set(bool _value);
    bool get();

private:
    bool value;
};

#endif // THREADSTOP_H
