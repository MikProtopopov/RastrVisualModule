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
    finish = procEnd;

    ui->progressBar->setParent(this);
    ui->progressBar->setValue(procStart);
    ui->progressBar->setMinimum(procStart);
    ui->progressBar->setMaximum(procEnd);

    connect(this, SIGNAL(pbSignal(int, int)), this, SLOT(pbUpdate(int, int)), Qt::BlockingQueuedConnection);

    factorial(procEnd);
    runThread = QtConcurrent::run(this, &this->threadRunner, rastrSize, procStart,
                                  procEnd, rastrMax, threadNumber); // REMOVE FACT RESULT
}

ProgressForm::~ProgressForm()
{
    runThread.waitForFinished();
    delete ui;
}

void ProgressForm::threadRunner(int n, int start, int end, int max, int threadNum)
{
    RastrManipulation rastrManipulation;
    Algorithm algorithm;
    ThreadStop threadStop;
    QObject::connect(this, SIGNAL(stopThread()),&threadStop,SLOT(markThread2Stop()),Qt::BlockingQueuedConnection);

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

    memset(rastrManipulation.rastr1[0], 0, n);
    for (int i=1; i<n; i++)
        rastrManipulation.rastr1[i][0] = 0;

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

    int foundCount=0;

    for (int i=start; i<end; i++)
    {
        memmove(algorithm.arrTemp2,algorithm.arrTempStart,n*sizeof(int));

        do{
            if (rastrManipulation.compareShlishevsky(max))
            {
                rastrManipulation.saveRastr(dirPath + "/" + filePath + "_" + QString::number(threadNum) +
                                              "_" + QString::number(foundCount) + ".rastr");
                foundCount++;
            }
            emit pbSignal(i+1,foundCount);
            if (threadStop.stopThread)
            {
                rastrManipulation.deleteArray(rastrManipulation.iRastr);
                delete algorithm.arrTemp1;
                delete algorithm.arrTemp2;
                delete algorithm.arrTemp;
                delete algorithm.arrTempStart;
                delete algorithm.localRastr;
                return;
            }
        }while(algorithm.NextSetCol());

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

void ProgressForm::pbUpdate(int i, int count)
{
    ui->progressBar->setValue(i);
    ui->label_2->setText(QString::number(count));
    QCoreApplication::processEvents();
}

void ProgressForm::on_pushButton_3_clicked()
{
    emit stopThread();
    delete this;
}
