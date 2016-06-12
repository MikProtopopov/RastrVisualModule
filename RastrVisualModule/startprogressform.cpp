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

#include "startprogressform.h"
#include "ui_startprogressform.h"
#include "progressform.h"

#include <QFileDialog>
#include <QMessageBox>

StartProgressForm::StartProgressForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartProgressForm)
{
    ui->setupUi(this);
    pForm = NULL;


    ui->lineEdit_2->setText("1");

}

StartProgressForm::~StartProgressForm()
{
    delete ui;
}

void StartProgressForm::on_pushButton_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Выберите место для сохранения результатов","Папка");

    ui->lineEdit->setText(dirName);
    QString fileName = "rastr123";
    savePath = dirName + "/" + fileName;
}

int StartProgressForm::setNumber(int i, int j)
{
    lineNumber = i;
    maximumCount = j;
}

void StartProgressForm::on_pushButton_2_clicked()
{
    if ("" == ui->lineEdit->text())
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Необходимо задать путь сохранения результатов."));
        return;
    }

    if ("" == ui->lineEdit_3->text())
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Необходимо задать префикс файлов."));
        return;
    }

    if ("" != ui->lineEdit_2->text())
        threadCount = ui->lineEdit_2->text().toInt();
    else
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Необходимо задать количество потоков."));
        return;
    }

    for (int i=0; i<ui->lineEdit_2->text().toInt(); i++)
    {
        formVector.append(new ProgressForm(this, lineNumber, 0, lineNumber, maximumCount, ui->lineEdit->text(), i, ui->lineEdit_3->text()));
        connect(formVector.last(),SIGNAL(markForm2Delete(ProgressForm *form)),this,SLOT(deletePF(ProgressForm *form)),Qt::DirectConnection);
        formVector.last()->show();
    }
}

void StartProgressForm::deletePF(ProgressForm *form )
{
    form->close();
    disconnect(form,SIGNAL(markForm2Delete(ProgressForm *form )),this,SLOT(deletePF(ProgressForm *form )));
    delete form;
    formVector.remove(formVector.indexOf(form));
}

/*int formNum*/
