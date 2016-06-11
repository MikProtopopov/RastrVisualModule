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
    explicit ProgressForm(QWidget *parent = 0, int rastrSize = 0, int procStart = 0, int procEnd = 0);
    ~ProgressForm();
    void setLimits(int begin, int end);
    QString numberString;

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
        factResult = 1;
        for (int i=1; i<=n; i++)
            factResult *= i;
        return factResult;
    }

    int factResult;

    void threadRunner(int n, int start, int end);


    QFuture<void> runThread;

    int start;
    int finish;
};

#endif // PROGRESSFORM_H
