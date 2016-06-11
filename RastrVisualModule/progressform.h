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

#ifndef PROGRESSFORM_H
#define PROGRESSFORM_H

#include <QDialog>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QProgressBar>
#include <QPushButton>
#include <stdint.h>


namespace Ui {
class ProgressForm;
}

class ProgressForm : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressForm(QWidget *parent = 0);
    ~ProgressForm();
    void setLimits(int begin, int end);
    int number;
    void setNumber(int i);

public slots:
    void pbUpdate(int i);

signals:
    pbSignal(int i);

private slots:
    void on_pushButton_3_clicked();

private:
    Ui::ProgressForm *ui;
    QString filePath;

    int factorial(int n)
    {
        int k = 1;
        for (int i=1; i<=n; i++)
            k = k * i;
        return k;
    }

    void threadRunner(int n, int start, int end);


    QFuture<void> runThread;

    int start;
    int finish;
};

class Algorithm
{
public:
    uint8_t **localRastr;
    int *arrTemp1;
    int *arrTemp2;
    int *arrTempStart;
    int rastrSize;

    void swapCol(uint8_t **&a, int i, int j, int N);
    void swapRow(uint8_t **&a, int i, int j);
    void swap(int *a, int i, int j);
    bool NextSetCol();
    bool NextSetRow();

};

#endif // PROGRESSFORM_H
