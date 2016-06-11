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
#include "algorithm.h"

#include <math.h>

ProgressForm::ProgressForm(QWidget *parent, int rastrSize, int procStart, int procEnd) :
    QDialog(parent),
    ui(new Ui::ProgressForm)
{
    ui->setupUi(this);
    filePath = "";
    ui->pushButton_3->setText("Остановить");
    numberString = QString::number(rastrSize);

    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);

    connect(this, SIGNAL(pbSignal(int)), this, SLOT(pbUpdate(int)), Qt::DirectConnection);

    factorial(rastrSize);
    runThread = QtConcurrent::run(this, &this->threadRunner, rastrSize, procStart, factResult /*procEnd*/); // REMOVE FACT RESULT
}

ProgressForm::~ProgressForm()
{
    delete ui;
}

void ProgressForm::threadRunner(int n, int start, int end)
{
    RastrManipulation rastrManipulation;
    Algorithm algorithm;

    algorithm.rastrSize = n;

    algorithm.arrTempStart = new int[n];
    for (int i=0; i<n; i++)
        algorithm.arrTempStart[i] = i;

    algorithm.arrTemp1 = new int[n];
    memmove((char *)algorithm.arrTemp1, (char *)algorithm.arrTempStart,n*sizeof(int));
    algorithm.arrTemp2 = new int[n];

    rastrManipulation.setOscillation(1);
    rastrManipulation.createNewRastrAdamar(n);

    algorithm.localRastr = new uint8_t*[n];
    for (int i=0; i<n; i++)
        algorithm.localRastr[i] = new uint8_t[n];

    for (int i=0; i<n; i++)
        memmove(algorithm.localRastr[i],rastrManipulation.rastr1[i],n*sizeof(int));

    rastrManipulation.deleteArray(rastrManipulation.iRastr);

    rastrManipulation.rastr1 = new uint8_t*[n+1];
    rastrManipulation.rastr1[0] = new uint8_t[n];
    memset(rastrManipulation.rastr1[0], 0, n);

    for (int i=0; i<n; i++)
        rastrManipulation.rastr1[i+1] = algorithm.localRastr[i];

    rastrManipulation.iRastr = n+1;
    rastrManipulation.jRastr = n;

    rastrManipulation.fillRastr2();



    ui->label->setText(QString::number(start));
    ui->label_2->setText(QString::number(end));


//    for (int i=0; i<start; i++)
//        if (!algorithm.NextSetRow())
//        {
//            rastrManipulation.deleteArray(rastrManipulation.iRastr);
//            delete algorithm.arrTemp1;
//            delete algorithm.arrTemp2;
//            delete algorithm.arrTempStart;
//            delete algorithm.localRastr;
//            return;
//        }
    for (int i=start; i<end; i++)
    {
        memmove((char *)algorithm.arrTemp2,(char *)algorithm.arrTempStart,n*sizeof(int));
        emit pbSignal(i); // * 100 / (end+1)
        ui->label->setText(QString::number(i));
        if (!algorithm.NextSetRow())
            break;
    }

//    while (algorithm.NextSetRow())
//    {
//        for (int j=0; j<n; j++)
//            algorithm.arrTemp2[j] = j;

//        emit pbSignal(10);
//        Sleep(500);

//        if (i < end)
//            while (algorithm.NextSetCol())
//            {
//                i++;
//                emit pbSignal(i);
//            }
//    }
    rastrManipulation.deleteArray(n);
    delete algorithm.arrTemp1;
    delete algorithm.arrTemp2;
    delete algorithm.arrTempStart;
    delete algorithm.localRastr;
    return;
}

void ProgressForm::pbUpdate(int i)
{
    ui->progressBar->setValue(i);
    Sleep(500);
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
