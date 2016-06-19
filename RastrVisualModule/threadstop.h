#ifndef THREADSTOP_H
#define THREADSTOP_H

#include <QObject>


class ThreadStop : public QObject
{
public:
    ThreadStop();
    bool stopThread = false;
    void set(T_value);
    bool get();


public slots:
    void markThread2Stop();

private:
    QMutex *mutex;
    bool value;
};

#endif // THREADSTOP_H
