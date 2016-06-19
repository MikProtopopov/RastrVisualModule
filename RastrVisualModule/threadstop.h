#ifndef THREADSTOP_H
#define THREADSTOP_H


class ThreadStop
{
public:
    ThreadStop();
    bool stopThread = false;

public slots:
    void markThread2Stop();
};

#endif // THREADSTOP_H
