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

#ifndef STARTPROGRESSFORM_H
#define STARTPROGRESSFORM_H

#include <QDialog>
#include "progressform.h"

namespace Ui {
class StartProgressForm;
}

class StartProgressForm : public QDialog
{
    Q_OBJECT

public:
    explicit StartProgressForm(QWidget *parent = 0);
    ~StartProgressForm();
    int lineNumber;
    int maximumCount;
    int setNumber(int i, int j);
    QVector<ProgressForm *> formVector;

public slots:
    void deletePF(ProgressForm *form );

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::StartProgressForm *ui;
    ProgressForm *pForm;

    int threadCount;
    QString savePath;
};

#endif // STARTPROGRESSFORM_H
