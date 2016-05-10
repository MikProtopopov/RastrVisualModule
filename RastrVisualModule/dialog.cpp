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

#include "dialog.h"
#include "ui_dialog.h"
#include <QCloseEvent>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText("0");
    startStep = -1;

//    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(close()));

    setResult(Rejected);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    oscillation = ui->comboBox_2->currentIndex();
    mode = ui->comboBox->currentIndex();
    startStep = ui->lineEdit->text().toInt();
    setResult(Accepted);

    Dialog::setVisible(0);
}

void Dialog::setMode(int m)
{
    mode = m;
}

