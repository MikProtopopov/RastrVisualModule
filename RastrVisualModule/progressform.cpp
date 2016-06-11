// Copyright 2016, Protopopov Mikhail G. <mikprotopopov@yandex.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "progressform.h"
#include "ui_progressform.h"
#include "mainwindow.h"
#include "rastrmanipulation.h"

#include <math.h>

ProgressForm::ProgressForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressForm)
{
    ui->setupUi(this);
    filePath = "";
    ui->pushButton_3->setText("Остановить");

    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);

    connect(this, SIGNAL(pbSignal(int)), this, SLOT(pbUpdate(int)), Qt::DirectConnection);

    runThread = QtConcurrent::run(this, &this->threadRunner, number, 0, factorial(number));
}

ProgressForm::~ProgressForm()
{
    delete ui;
}

void ProgressForm::setNumber(int i)
{
    number = i;
}

void ProgressForm::threadRunner(int n, int start, int end)
{
    RastrManipulation rastrManipulation;
    Algorithm algorithm;

    algorithm.arrTemp1 = new int[n];
    for (int i=0; i<n; i++)
        algorithm.arrTemp1[i] = i;

    algorithm.arrTemp2 = new int[n];
    for (int i=0; i<n; i++)
        algorithm.arrTemp2[i] = i;

    rastrManipulation.setOscillation(1);
    rastrManipulation.createNewRastrAdamar(n);

    algorithm.localRastr = new uint8_t*[n];
    for (int i=0; i<n; i++)
        algorithm.localRastr[i] = new uint8_t[n];

    for (int i=0; i<n; i++)
        memmove(algorithm.localRastr[i],rastrManipulation.rastr1[i],n);

    rastrManipulation.deleteArray(n);

    rastrManipulation.rastr1 = new uint8_t*[n+1];
    rastrManipulation.rastr1[0] = new uint8_t[n];
    memset(rastrManipulation.rastr1[0], 0, n);

    for (int i=0; i<n; i++)
        rastrManipulation.rastr1[i+1] = algorithm.localRastr[i];

    rastrManipulation.iRastr = n+1;
    rastrManipulation.jRastr = n;

    rastrManipulation.fillRastr2();

    int i = 0;
    for (int j=0; j < start; j++)
        algorithm.NextSetRow(algorithm.localRastr, n, algorithm.arrTemp1);

    while (algorithm.NextSetRow(algorithm.localRastr, n, algorithm.arrTemp1))
    {
        for (int j=0; j<n; j++)
            algorithm.arrTemp2[j] = j;

        emit pbSignal(10);
        Sleep(500);

        if (i < end)
            while (algorithm.NextSetCol(algorithm.localRastr, n, algorithm.arrTemp2))
            {
                i++;
                emit pbSignal(20 + i);
            }
    }
    rastrManipulation.deleteArray(n);
    return;
}

void ProgressForm::pbUpdate(int i)
{
    ui->progressBar->setValue(i);
    Sleep(300);
    return;
}

void ProgressForm::setLimits(int begin, int end)
{
    start = begin;
    finish = end;
}

void ProgressForm::on_pushButton_3_clicked()
{
//    runThread = QtConcurrent::run(this, &this->threadRunner, number);
}

void Algorithm::swapCol(uint8_t **&a, int i, int j, int N)
{
    for (int k=0; k<N; k++)
    {
        int s = a[k][i];
        a[k][i] = a[k][j];
        a[k][j] = s;
    }
}

void Algorithm::swap(int *a, int i, int j) {
  int s = a[i];
  a[i] = a[j];
  a[j] = s;
}

void Algorithm::swapRow(uint8_t **&a, int i, int j)
{
        uint8_t *arrTemp = a[i];
        a[i] = a[j];
        a[j] = arrTemp;
}

bool Algorithm::NextSetCol(uint8_t **&a, int n, int *&arrTemp)
{
        int j = n - 2;
        while (j != -1 && arrTemp[j] >= arrTemp[j + 1])
            j--;
        if (j == -1)
            return false; // больше перестановок нет
        int k = n - 1;
        while (arrTemp[j] >= arrTemp[k])
            k--;
        swapCol(a, j, k, n);
        swap(arrTemp,j,k);
        int l = j + 1, r = n - 1; // сортируем оставшуюся часть последовательности
        while (l<r)
        {
            swapCol(a, l, r,n);
            swap(arrTemp,l++,r--);
        }
        return true;
}

bool Algorithm::NextSetRow(uint8_t **&a, int n, int *&arrTemp)
{
        int j = n - 2;
        while (j != -1 && arrTemp[j] >= arrTemp[j + 1])
            j--;
        if (j == -1)
            return false; // больше перестановок нет
        int k = n - 1;
        while (arrTemp[j] >= arrTemp[k])
            k--;
        swapRow(a, j, k);
        swap(arrTemp,j,k);
        int l = j + 1, r = n - 1; // сортируем оставшуюся часть последовательности
        while (l<r)
        {
            swapRow(a, l, r);
            swap(arrTemp,l++,r--);
        }
        return true;
}
