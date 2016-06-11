#include "algorithm.h"

void Algorithm::swapCol(int i, int j)
{
    for (int k=0; k<rastrSize; k++)
    {
        int s = localRastr[k][i];
        localRastr[k][i] = localRastr[k][j];
        localRastr[k][j] = s;
    }

    int s = arrTemp2[i];
    arrTemp2[i] = arrTemp2[j];
    arrTemp2[j] = s;
}

void Algorithm::swapRow(int i, int j)
{
        uint8_t *arrTemp = localRastr[i];
        localRastr[i] = localRastr[j];
        localRastr[j] = arrTemp;

        int s = arrTemp1[i];
        arrTemp1[i] = arrTemp1[j];
        arrTemp1[j] = s;
}

bool Algorithm::NextSetCol()
{
  int j = rastrSize - 2;
  while (j != -1 && arrTemp2[j] >= arrTemp2[j + 1]) j--;
  if (j == -1)
    return false; // больше перестановок нет
  int k = rastrSize - 1;
  while (arrTemp2[j] >= arrTemp2[k]) k--;
  swapCol(j,k);
  int l = j + 1, r = rastrSize - 1; // сортируем оставшуюся часть последовательности
  while (l<r)
    swapCol(l++,r--);
  return true;
}

bool Algorithm::NextSetRow()
{
        int j = rastrSize - 2;
        while (j != -1 && arrTemp1[j] >= arrTemp1[j + 1])
            j--;
        if (j == -1)
            return false; // больше перестановок нет
        int k = rastrSize - 1;
        while (arrTemp1[j] >= arrTemp1[k])
            k--;
        swapRow(j, k);
        int l = j + 1, r = rastrSize - 1; // сортируем оставшуюся часть последовательности
        while (l<r)
            swapRow(l++, r--);
        return true;
}
