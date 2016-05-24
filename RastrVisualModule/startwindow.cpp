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

#include "startwindow.h"
#include "ui_startwindow.h"
#include "mainwindow.h"
#include "rastrmanipulation.h"

StartWindow::StartWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    if (0 == ui->comboBox->currentIndex())
    {
        ui->lineEditY->setEnabled(1);
        ui->textBrowser->setText("Случайная матрица, создаваемая по заданным параметрам ширины и высоты.");
    }
    if (1 == ui->comboBox->currentIndex())
    {
        ui->lineEditY->setEnabled(0);
        ui->textBrowser->setText("Квадратная матрица, заданная по параметру ширины. Заполняется согласну определенному алгоритму.");
    }
    if (2 == ui->comboBox->currentIndex())
    {
        ui->lineEditY->setEnabled(0);
        ui->textBrowser->setText("Матрица, построенная по алгоритмам Мурзиной.");
    }
    if (3 == ui->comboBox->currentIndex())
    {
        ui->lineEditY->setEnabled(1);
        ui->textBrowser->setText("Матрица, построенная по алгоритмам Шлишевского.");
    }

    ui->lineEditX->setValidator(new QRegExpValidator(QRegExp("[0-9]{1,10}"),this));
    ui->lineEditY->setValidator(new QRegExpValidator(QRegExp("[0-9]{1,10}"),this));

}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::on_pushButton_clicked()
{
    width = ui->lineEditX->text().toInt();
    if (0 == ui->comboBox->currentIndex())
        height = ui->lineEditY->text().toInt();
    if (1 == ui->comboBox->currentIndex())
        height = -1;
    matrixType = ui->comboBox->currentIndex();
    if (((width > 100)||(height > 100)||(width < 3)||(height < 3))&&((matrixType != 1)||(matrixType != 2)))
    {
        setResult(Rejected);
        QMessageBox::information(this, tr("Ошибка"), tr("Введены неверные параметры."));
    }
    else
    {
        setResult(Accepted);
        StartWindow::setVisible(0);
    }
}

void StartWindow::on_comboBox_currentIndexChanged(int index)
{
    if (0 == index)
    {
        ui->lineEditY->setEnabled(1);

        ui->label->setText("Высота");
        ui->textBrowser->setText("Случайная матрица, создаваемая по заданным параметрам ширины и высоты.");
    }
    if (1 == index)
    {
        ui->lineEditY->setEnabled(0);
        ui->label->setText(" ");
        ui->textBrowser->setText("Квадратная матрица, заданная по параметру ширины. Заполняется согласну определенному алгоритму.");
    }
    if (2 == index)
    {
        ui->lineEditY->setEnabled(0);
        ui->label->setText(" ");
        ui->textBrowser->setText("Матрица, построенная по алгоритмам Мурзиной.");
    }
    if (3 == index)
    {
        ui->lineEditY->setEnabled(1);
        ui->label->setText("Максимум");
        ui->textBrowser->setText("Матрица, построенная по алгоритмам Шлишевского.");
    }
}

int StartWindow::getHeight()
{
    return height;
}

int StartWindow::getWidth()
{
    return width;
}

int StartWindow::getMatrixType()
{
    return matrixType;
}
