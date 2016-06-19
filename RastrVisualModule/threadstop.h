#ifndef THREADSTOP_H
#define THREADSTOP_H

#include <QObject>


class ThreadStop : public QObject
{
public:
    ThreadStop(QMutex *_mutex, bool _value);
    bool stopThread = false;
    void set(bool _value);
    bool get();


public slots:
    void markThread2Stop();

private:
    QMutex *mutex;
    bool value;
};

#endif // THREADSTOP_H
