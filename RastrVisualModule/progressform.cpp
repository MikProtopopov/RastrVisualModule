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

ProgressForm::ProgressForm(QWidget *parent, int rastrSize, int procStart,
                           int procEnd, int maximum, QString path, int tNum, QString name) :
    QDialog(parent),
    ui(new Ui::ProgressForm)
{
    ui->setupUi(this);
    dirPath = path;
    filePath = name;
    ui->pushButton_3->setText("Остановить");
    numberString = QString::number(rastrSize);
    rastrMax = maximum;
    threadNumber = tNum;

    ui->progressBar->setParent(this);
    ui->progressBar->setValue(procStart);
    ui->progressBar->setMinimum(procStart);
    ui->progressBar->setMaximum(procEnd);

    connect(this, SIGNAL(pbSignal(int)), this, SLOT(pbUpdate(int)), Qt::DirectConnection);

    factorial(procEnd);
    runThread = QtConcurrent::run(this, &this->threadRunner, rastrSize, procStart,
                                  procEnd, rastrMax, threadNumber); // REMOVE FACT RESULT
}

ProgressForm::~ProgressForm()
{
    delete ui;
}

void ProgressForm::threadRunner(int n, int start, int end, int max, int threadNum)
{
    RastrManipulation rastrManipulation;
    Algorithm algorithm;

    algorithm.rastrSize = n;

    algorithm.arrTempStart = new int[n];
    for (int i=0; i<n; i++)
        algorithm.arrTempStart[i] = i;

    algorithm.arrTemp = new uint8_t[n];
    algorithm.n = n;


    algorithm.arrTemp1 = new int[n];
    memmove(algorithm.arrTemp1, algorithm.arrTempStart,n*sizeof(int));
    algorithm.arrTemp2 = new int[n];

    rastrManipulation.setOscillation(1);
    rastrManipulation.createNewRastrAdamar(n);

    algorithm.localRastr = new uint8_t*[n];
    for (int i=0; i<n; i++)
        algorithm.localRastr[i] = new uint8_t[n];

    for (int i=0; i<n; i++)
        memmove(algorithm.localRastr[i],rastrManipulation.rastr1[i],n);

    rastrManipulation.deleteArray(rastrManipulation.iRastr);

    rastrManipulation.rastr1 = new uint8_t*[n+1];
    rastrManipulation.rastr1[0] = new uint8_t[n];
    memset(rastrManipulation.rastr1[0], 0, n);

    for (int i=0; i<n; i++)
        rastrManipulation.rastr1[i+1] = algorithm.localRastr[i];

    rastrManipulation.iRastr = n+1;
    rastrManipulation.jRastr = n;

    rastrManipulation.fillRastr2();

    for (int i=0; i<start; i++)
        if (!algorithm.NextSetRow())
        {
            rastrManipulation.deleteArray(rastrManipulation.iRastr);
            delete algorithm.arrTemp1;
            delete algorithm.arrTemp2;
            delete algorithm.arrTemp;
            delete algorithm.arrTempStart;
            delete algorithm.localRastr;
            return;
        }

    int k=0;

    for (int i=start; i<end; i++)
    {
        memmove(algorithm.arrTemp2,algorithm.arrTempStart,n*sizeof(int));

        do{
            k++;
//            if (rastrManipulation.compareShlishevsky(max))
                rastrManipulation.exportRastr(dirPath + "/" + filePath + "_" + QString::number(threadNum) +
                                              "_" + QString::number(k) + ".txt");
        }while(algorithm.NextSetCol());
            emit pbSignal((i-start)/(end-start)*100);

        if (!algorithm.NextSetRow())
            break;
    }

    rastrManipulation.deleteArray(n);
    delete algorithm.arrTemp1;
    delete algorithm.arrTemp2;
    delete algorithm.arrTemp;
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
    emit pbSignal(0);
    emit markForm2Delete(this);
}
