#include "threadstop.h"

ThreadStop::ThreadStop()
{

}

void ThreadStop::markThread2Stop()
{
    stopThread = true;
}
