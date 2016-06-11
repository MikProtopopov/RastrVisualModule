#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <stdint.h>


class Algorithm
{
public:
    uint8_t **localRastr;
    int *arrTemp1;
    int *arrTemp2;
    int *arrTempStart;
    int rastrSize;

    void swapCol(int i, int j);
    void swapRow(int i, int j);
    bool NextSetCol();
    bool NextSetRow();

};

#endif // ALGORITHM_H
