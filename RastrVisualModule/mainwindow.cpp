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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "startwindow.h"
#include "rastrmanipulation.h"
#include "paintrastr.h"
#include "qcustomplot.h"
#include "helpwindow.h"
#include "startprogressform.h"

#include <malloc.h>
#include <fstream>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QDataStream>
#include <QMessageBox>
#include <QString>
#include <QMenu>
#include <QMenuBar>
#include <QIODevice>
#include <cstdlib>
#include <stdlib.h>
#include <QWidget>
#include <QtConcurrent/QtConcurrentRun>

using namespace std;
// Constructor for MainWindow
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dialog = NULL;
    sWindow = NULL;
    paintRastr1 = NULL;
    paintRastr2 = NULL;
    sPForm = NULL;

    try
    {
        dialog = new Dialog(this);
        sWindow = new StartWindow(this);
        helpWindow = new HelpWindow(this);
        paintRastr1 = new PaintRastr(this);
        paintRastr2 = new PaintRastr(this);
        sPForm = new StartProgressForm(this);
    } catch (...){
        QMessageBox::information(this, tr("Ошибка"), tr("Недостаточно памяти для работы программы."));
        if (NULL != dialog)
            dialog->deleteLater();
        if (NULL != sWindow)
            sWindow->deleteLater();
        if (NULL != paintRastr1)
            paintRastr1->deleteLater();
        if (NULL != paintRastr2)
            paintRastr2->deleteLater();
        if (NULL != sPForm)
            sPForm->deleteLater();
        exit(0);
    }

    ui->verticalLayout->addWidget(paintRastr1);
    ui->verticalLayout_2->addWidget(paintRastr2);

    // Hiding window counters
    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();

    // Hiding plots for graphs
    ui->customPlot1->hide();
    ui->customPlot2->hide();
    ui->customPlot3->hide();

    // Adding graphs on plots
    ui->customPlot1->addGraph();
    ui->customPlot2->addGraph();
    ui->customPlot3->addGraph();

    // Adding first (0) element to vectors
    graphX.append(0);
    graphY.append(0);
    graphXOsci.append(0);
    graphYOsci.append(0);
    graphXComp.append(0);
    graphYComp.append(0);

    // Setting first point in graphs
    ui->customPlot1->graph(0)->setData(graphX,graphY);
    ui->customPlot2->graph(0)->setData(graphXOsci,graphYOsci);
    ui->customPlot3->graph(0)->setData(graphXComp,graphYComp);

    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(on_actionExport_clicked()));
    connect(ui->actionImport, SIGNAL(triggered()), this, SLOT(on_actionImport_clicked()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(on_actionNew_clicked()));
    connect(ui->actionManual, SIGNAL(triggered()), helpWindow, SLOT(loadHelp()));
    connect(this, SIGNAL(stepSignal()), this, SLOT(oneStep()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(stepSignalOsci()), this, SLOT(oneStepOsci()), Qt::BlockingQueuedConnection);

}

MainWindow::~MainWindow()
{
    paintRastr1->deleteLater(); // Clearing paintRastr1 from memory
    paintRastr2->deleteLater(); // Clearing paintRastr2 from memory
    delete ui;                  // Clearing UI from memory
}

// For resize handling
void MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    int margin = 10;
    int anchor1 = (this->height()-20) / 2;     // Height of the draw area for rastr
    int anchor2 = this->height() - anchor1 - margin*3 - ui->pushButtonStart->height() - margin*5; // Height of the draw area for graphs
    int anchor3 = (this->width()- margin*4) / 3; // Width of the draw areas for graphs

    ui->graphicsView_1->resize(this->width()-20, anchor1); // Top window resize

    // Rastr draw areas resize
    ui->verticalLayoutWidget->resize(this->width()-20, anchor1);
    ui->verticalLayoutWidget_2->resize(this->width()-20, anchor1);

    // Update of the size of the window
    paintRastr1->setSize(ui->graphicsView_1->height(), ui->graphicsView_1->width());
    paintRastr2->setSize(ui->graphicsView_1->height(), ui->graphicsView_1->width());

    // Refresh the rastr drawings
    paintRastr1->update();
    paintRastr2->update();

    // Resize the leftmost graph draw area
    ui->graphicsView_2->move(margin, margin + anchor1 + margin + ui->pushButtonStart->height() + margin);
    ui->graphicsView_2->resize(anchor3, anchor2);

    // Resize the middle graph draw area
    ui->graphicsView_3->move(margin + anchor3 + margin, margin + anchor1 + margin + ui->pushButtonStart->height() + margin);
    ui->graphicsView_3->resize(anchor3, anchor2);

    // Resize the rightmost graph draw area
    ui->graphicsView_4->move(margin + anchor3*2 + margin*2, margin + anchor1 + margin + ui->pushButtonStart->height() + margin);
    ui->graphicsView_4->resize(anchor3, anchor2);

    // Resize the leftmost graph draw area
    ui->customPlot1->move(margin, margin + anchor1 + margin + ui->pushButtonStart->height() + margin);
    ui->customPlot1->resize(anchor3, anchor2);
    ui->label->move(margin*2, margin + anchor1 + ui->pushButtonStart->height() + anchor2);

    // Resize the middle graph draw area
    ui->customPlot2->move(margin + anchor3 + margin, margin + anchor1 + margin + ui->pushButtonStart->height() + margin);
    ui->customPlot2->resize(anchor3, anchor2);
    ui->label_2->move(margin + anchor3 + margin*2, margin + anchor1 + ui->pushButtonStart->height() + anchor2);

    // Resize the rightmost graph draw area
    ui->customPlot3->move(margin + anchor3*2 + margin*2, margin + anchor1 + margin + ui->pushButtonStart->height() + margin);
    ui->customPlot3->resize(anchor3, anchor2);
    ui->label_3->move(margin + anchor3*2 + margin*3, margin + anchor1 + ui->pushButtonStart->height() + anchor2);

    // Movement of buttons
    ui->pushButtonStart->move(margin, margin + anchor1 + margin);
    ui->pushButtonStep->move(margin + ui->pushButtonStart->width() + margin, margin + anchor1 + margin);
    ui->pushButtonColor->move(this->width() - margin - ui->pushButtonColor->width(), margin + anchor1 + margin);

    ui->customPlot1->move(margin, margin + anchor1 + margin + ui->pushButtonStart->height() + margin);
}

// Function for drawing lines on non-oscillated graph
int MainWindow::drawGraph(QCustomPlot *customPlot)
{
    // Stop work if rastr, needed for work, does not exist
    if (NULL == rastrManipulation.rastr1)
        return 0;

    try {
        graphX.append(paintRastr2->stepMov); // Add value to vector for horizontal coordinates
        graphY.append(rastrManipulation.compareRastr(paintRastr2->stepMov,1)); // Add value to vector for vertical coordinates
    } catch(...){
        return 7;
    }

    customPlot->graph(0)->setData(graphX,graphY); // Sets point on the graph and connects it to the precious point
    customPlot->replot(); // Refreshes the plot
    return 0;
}

// Checking for save
void MainWindow::checkForSave()
{
    QMessageBox reply(QMessageBox::Question, tr("Несохраненные данные"), tr("Сохранить текущую матрицу?"),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
    reply.setButtonText(QMessageBox::Yes, tr("Да"));
    reply.setButtonText(QMessageBox::No, tr("Нет"));
    reply.setButtonText(QMessageBox::Cancel, tr("Отмена"));

    reply.exec();

    if (reply.result() == QMessageBox::Yes)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить"),
                                                         "", tr("Растровый файл (*.rastr);;Все файлы(*)")); // Call for a "save" window
        if (fileName.isEmpty())
             return;

        errorHandling(rastrManipulation.saveRastr(fileName));
    }
    if (reply.result() == QMessageBox::No)
        return;
}

void MainWindow::clearVectors()
{
    if (graphX.count() > 1)
    {
        graphX.clear();
        graphY.clear();

        graphX.append(0);
        graphY.append(0);

        ui->customPlot1->graph()->clearData();
        ui->customPlot1->graph(0)->setData(graphX,graphY);
        ui->customPlot1->hide();
        ui->label->hide();

        if (graphXOsci.count() > 1)
        {
            graphXOsci.clear();
            graphYOsci.clear();
            graphXComp.clear();
            graphYComp.clear();

            graphXOsci.append(0);
            graphYOsci.append(0);
            graphXComp.append(0);
            graphYComp.append(0);

            ui->customPlot2->graph()->clearData();
            ui->customPlot3->graph()->clearData();
            ui->customPlot2->graph(0)->setData(graphXOsci,graphYOsci);
            ui->customPlot3->graph(0)->setData(graphXComp,graphYComp);
            ui->customPlot2->hide();
            ui->customPlot3->hide();
            ui->label_2->hide();
            ui->label_3->hide();
        }
    }
}

// Function for drawing lines on oscillated graph
int MainWindow::drawGraphOsci(QCustomPlot *customPlot)
{
    // Stop work if rastr, needed for work, does not exist
    if (NULL == rastrManipulation.rastr1)
        return 0;

    try {
        graphXOsci.append(paintRastr2->stepMov); // Add value to vector for horizontal coordinates
        graphYOsci.append(rastrManipulation.compareRastr(paintRastr2->stepMov,0)); // Add value to vector for vertical coordinates
    } catch(...){
        return 7;
    }

    customPlot->graph(0)->setData(graphXOsci,graphYOsci); // Sets point on the graph and connects it to the precious point
    customPlot->replot();                                 // Refreshes the plot
    return 0;
}

// Function for drawing lines on comparison graph
int MainWindow::drawGraphCompare(QCustomPlot *customPlot)
{
    // Stop work if rastr, required for work, does not exist
    if (NULL == rastrManipulation.rastr1)
        return 0;

    try {
        graphXComp.append(paintRastr2->stepMov); // Add value to vector for horizontal coordinates
        // Difference between rastrs
//        graphYComp.append((abs(rastrManipulation.compareRastr(paintRastr2->stepMov,1) - rastrManipulation.compareRastr(paintRastr2->stepMov,0))
//                    + (rastrManipulation.compareRastr(paintRastr2->stepMov,1) - rastrManipulation.compareRastr(paintRastr2->stepMov,0))) / 2);
        graphYComp.append(rastrManipulation.compareRastr(paintRastr2->stepMov,1) - rastrManipulation.compareRastr(paintRastr2->stepMov,0));
    } catch(...){
        return 7;
    }

    customPlot->graph(0)->setData(graphXComp,graphYComp); // Sets point on the graph and connects it to the precious point
    customPlot->replot(); // Refreshes the plot
    return 0;
}

// Error Processing Facility
void MainWindow::errorHandling(int errorCode) // Takes an error code and matches it with known errors. Sends message out about this error
{
    switch(errorCode)
    {
    case 0:
        break;
    case 2:
        QMessageBox::information(this, tr("Ошибка"), tr("Не удалось открыть файл."));
        break;
    case 3:
        QMessageBox::information(this, tr("Ошибка"), tr("Неверное количество строк."));
        break;
    case 4:
        QMessageBox::information(this, tr("Ошибка"), tr("Неверное число элементов в строке."));
        break;
    case 5:
        QMessageBox::information(this, tr("Ошибка"), tr("Некорректный элемент в строке."));
        break;
    case 6:
        QMessageBox::information(this, tr("Ошибка"), tr("Имя файла пустое."));
        break;
    case 7:
        QMessageBox::information(this, tr("Ошибка"), tr("Сбой в построении графика."));
        break;
    case 8:
        QMessageBox::information(this, tr("Ошибка"), tr("Сбой в удалении."));
        break;
    case 9:
        QMessageBox::information(this, tr("Ошибка"), tr("Сбой в создании."));
        break;
    case 10:
        QMessageBox::information(this, tr("Ошибка"), tr("Сбой в заполнении."));
        break;
    case 11:
        QMessageBox::information(this, tr("Ошибка"), tr("Сбой в int=int."));
        rastrManipulation.deleteArray(rastrManipulation.iRastr);
        exit(0);
    }
}


// Triggers the start of rastr comparison
void MainWindow::on_pushButtonStart_clicked()
{
    int rastr2Openwindows; // maximum open windows counter

    dialog->exec();
    if (QDialog::Accepted != dialog->result())
        return;

    rastrManipulation.setOscillation(dialog->oscillation);

    if (NULL == rastrManipulation.rastr2)
        errorHandling(rastrManipulation.fillRastr2()); // Fills second, moving rastr

    paintRastr2->setBGColor(Qt::white);
    paintRastr2->setParameters(ui->graphicsView_1->height(), ui->graphicsView_1->width(),
                               rastrManipulation.iRastr - rastrManipulation.oscillation, rastrManipulation.jRastr,0, Qt::black,
                               rastrManipulation.iRastr, rastrManipulation.jRastr, 1); // Set parameters for moving rastr
    paintRastr2->setRastr(rastrManipulation.rastr2);

    clearVectors(); // Clearing vector for oscilated graph if not empty

    rastr2Openwindows = rastrManipulation.countWindows();

    // Determining the "length" of ticks on Y axis
    ui->customPlot1->yAxis->setTickStep((rastrManipulation.countWindows() + 2) / 6);
    ui->customPlot2->yAxis->setTickStep((rastrManipulation.countWindows() + 2) / 6);
    ui->customPlot3->yAxis->setTickStep((rastrManipulation.countWindows() + 2) / 6);

    // Setting range of numbers for Y axis
    ui->customPlot1->yAxis->setRange(0, rastrManipulation.countWindows() + 2);
    ui->customPlot2->yAxis->setRange(0, rastrManipulation.countWindows() + 2);
    ui->customPlot3->yAxis->setRange(0, rastrManipulation.countWindows() + 2);
    try {
        ui->customPlot1->show(); // Shows first graph on the main form
        ui->customPlot2->show();
        ui->customPlot3->show();
        ui->label->show();
        ui->label_2->show();
        ui->label_3->show();
    } catch (...) {
        QMessageBox::information(this, tr("Ошибка"), tr("Сбой в построении графика."));
        rastrManipulation.deleteArray(rastrManipulation.iRastr);
        exit(0);
    }

    errorHandling(drawGraph(ui->customPlot1)); // Draw line in graph 1
    if (1 == rastrManipulation.oscillation)
    {
        errorHandling(drawGraphOsci(ui->customPlot2)); // Draw line in graph 2
        errorHandling(drawGraphCompare(ui->customPlot3)); // Draw line in graph 3
    }

    paintRastr2->show();

    ui->pushButtonStep->setEnabled(1);

    if ((0 < dialog->startStep)&&(1 == dialog->mode))
    {
        dialog->setMode(0);
        for (int i=0; i < dialog->startStep; i++)
            ui->pushButtonStep->click();
        dialog->setMode(1);
    }

    if ((0 < dialog->startStep)&&(0 == dialog->mode))
    {
        for (int i=0; i < dialog->startStep; i++)
            ui->pushButtonStep->click();
    }
}

void MainWindow::on_actionNew_clicked()
{
    if (rastrManipulation.rastr1 != NULL)
        checkForSave();

    sWindow->exec();

    if (QDialog::Accepted != sWindow->result())
        return;

    if (((this->minimumHeight() - 20) / 2.1) / sWindow->getWidth() < 4)
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Слишком большая размерность матрицы."));
        return;
    }

    // Create standard randomized rastr
    if (sWindow->getMatrixType() == 0)
    {
        if (-1 == sWindow->getHeight())
            return;
        errorHandling(rastrManipulation.createNewRastr(sWindow->getWidth(),sWindow->getHeight()));
        ui->pushButtonStep->setEnabled(0); // Disables "Step" button
        ui->pushButtonStart->setEnabled(1); // Enables "Start" button
        ui->pushButtonColor->setEnabled(1);
    }

    // Create rastr based on Adamar matrix
    if (sWindow->getMatrixType() == 1)
    {
        rastrManipulation.createNewRastrAdamar(sWindow->getWidth());
        ui->pushButtonStep->setEnabled(0); // Disables "Step" button
        ui->pushButtonStart->setEnabled(1); // Enables "Start" button
        ui->pushButtonColor->setEnabled(1);
    }

    if (sWindow->getMatrixType() == 2)
    {
        rastrManipulation.createNewRastrMurzina(sWindow->getWidth());
        ui->pushButtonStep->setEnabled(0); // Disables "Step" button
        ui->pushButtonStart->setEnabled(1); // Enables "Start" button
        ui->pushButtonColor->setEnabled(1);
    }

    if (sWindow->getMatrixType() == 3)
    {
        sPForm->setNumber(sWindow->getWidth(), sWindow->getHeight());
        sPForm->exec();
//        sPForm->deleteLater();
        return;
    }


    paintRastr1->setParameters(ui->graphicsView_1->height(), ui->graphicsView_1->width(),
                               rastrManipulation.iRastr, rastrManipulation.jRastr,rastrManipulation.jRastr, Qt::gray,
                               rastrManipulation.iRastr, rastrManipulation.jRastr, 0); // Set parameters for background rastr
    paintRastr1->setBGColor(Qt::white);

    paintRastr1->setRastr(rastrManipulation.rastr1); // Sets matrix for background rastr

    clearVectors();

    paintRastr2->hide();

    // Set tick length for first graph
    ui->customPlot1->xAxis->setAutoTickStep(false);
    ui->customPlot1->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
    ui->customPlot1->yAxis->setAutoTickStep(false);

    // Set tick length for second graph
    ui->customPlot2->xAxis->setAutoTickStep(false);
    ui->customPlot2->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
    ui->customPlot2->yAxis->setAutoTickStep(false);

    // Set tick length for third graph
    ui->customPlot3->xAxis->setAutoTickStep(false);
    ui->customPlot3->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
    ui->customPlot3->yAxis->setAutoTickStep(false);

    // give the axes some labels:
    ui->customPlot1->xAxis->setLabel("AcF");
    ui->customPlot1->yAxis->setLabel("Количество открытых окон");
    // set axes ranges, so we see all data:
    ui->customPlot1->xAxis->setRange(0, rastrManipulation.jRastr*2);

    ui->customPlot2->xAxis->setLabel("AcF'");
    ui->customPlot2->yAxis->setLabel("Количество открытых окон");
    // set axes ranges, so we see all data:
    ui->customPlot2->xAxis->setRange(0, rastrManipulation.jRastr*2);

    ui->customPlot3->xAxis->setLabel("AcF' - AcF");
    ui->customPlot3->yAxis->setLabel("Количество открытых окон");
    // set axes ranges, so we see all data:
    ui->customPlot3->xAxis->setRange(0, rastrManipulation.jRastr*2);

    ui->actionExport->setEnabled(1);
    ui->actionSave->setEnabled(1);


}

// Triggers import of a rastr from a text file
void MainWindow::on_actionImport_clicked()
{
    if (rastrManipulation.rastr1 != NULL)
        checkForSave();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Импортировать"),
                                                    "", tr("Текстовый файл (*.txt);")); // Call for an "import" window
    if (fileName.isEmpty())
        return;

    errorHandling(rastrManipulation.importRastr(fileName));

    paintRastr2->hide();

    if (( (this->minimumHeight() - 20) / 2.1) / rastrManipulation.iRastr < 4)
    {
        QMessageBox::information(this, tr("Ошибка"), tr("Слишком большая размерность матрицы."));
        rastrManipulation.deleteArray(rastrManipulation.iRastr);
        return;
    }

    paintRastr1->setParameters(ui->graphicsView_1->height(), ui->graphicsView_1->width(),
                               rastrManipulation.iRastr, rastrManipulation.jRastr,rastrManipulation.jRastr, Qt::gray,
                               rastrManipulation.iRastr, rastrManipulation.jRastr, 0); // Set parameters for background rastr
    paintRastr1->setBGColor(Qt::white);


    paintRastr1->setRastr(rastrManipulation.rastr1); // Sets matrix for background rastr

    ui->pushButtonStep->setEnabled(0); // Disables "Step" button
    ui->pushButtonStart->setEnabled(1); // Enables "Start" button
    ui->pushButtonColor->setEnabled(1);

    clearVectors();

    // Set tick length for first graph
    ui->customPlot1->xAxis->setAutoTickStep(false);
    ui->customPlot1->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
    ui->customPlot1->yAxis->setAutoTickStep(false);

    // Set tick length for second graph
    ui->customPlot2->xAxis->setAutoTickStep(false);
    ui->customPlot2->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
    ui->customPlot2->yAxis->setAutoTickStep(false);

    // Set tick length for third graph
    ui->customPlot3->xAxis->setAutoTickStep(false);
    ui->customPlot3->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
    ui->customPlot3->yAxis->setAutoTickStep(false);

    // give the axes some labels:
    ui->customPlot1->xAxis->setLabel("AcF");
    ui->customPlot1->yAxis->setLabel("Количество открытых окон");
    // set axes ranges, so we see all data:
    ui->customPlot1->xAxis->setRange(0, rastrManipulation.jRastr*2);

    ui->customPlot2->xAxis->setLabel("AcF'");
    ui->customPlot2->yAxis->setLabel("Количество открытых окон");
    // set axes ranges, so we see all data:
    ui->customPlot2->xAxis->setRange(0, rastrManipulation.jRastr*2);

    ui->customPlot3->xAxis->setLabel("AcF' - AcF");
    ui->customPlot3->yAxis->setLabel("Количество открытых окон");
    // set axes ranges, so we see all data:
    ui->customPlot3->xAxis->setRange(0, rastrManipulation.jRastr*2);

    ui->actionExport->setEnabled(1);
    ui->actionSave->setEnabled(1);
}

// Triggers export from a rastr into a text file
void MainWindow::on_actionExport_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Экспортировать"),
                                                    "", tr("Текстовый файл (*.txt);;Все файлы(*)")); // Call for an "export" window
    if (fileName.isEmpty())
        return;

    errorHandling(rastrManipulation.exportRastr(fileName));
}

// TODO Try using QtConcurrent to get rid of the freeze
// Triggers movement of the moving rastr

void MainWindow::oneStep()
{
    if (paintRastr2->stepMov >= rastrManipulation.jRastr * 2)
        return;

    if (paintRastr2->stepMov < rastrManipulation.jRastr * 2)
    {
        paintRastr2->stepMov += 1;                 // Representation of a Step
        errorHandling(drawGraph(ui->customPlot1)); // Draw line in graph 1
        ui->label->setText("Открытых окон: " + QString::number(graphY.last()));
        paintRastr2->update();
    }
    return;
}

void MainWindow::oneStepOsci()
{
    if ((paintRastr2->stepMov >= rastrManipulation.jRastr * 2)&&(0 == paintRastr2->oStatus))
        return;

    if (paintRastr2->stepMov % 2 != paintRastr2->oStatus)
    {
        paintRastr2->oStatus = !paintRastr2->oStatus;
        errorHandling(drawGraphOsci(ui->customPlot2));    // Draw line in graph 2
        errorHandling(drawGraphCompare(ui->customPlot3)); // Draw line in graph 3
        ui->label_2->setText("Открытых окон: " + QString::number(graphYOsci.last()));
        ui->label_3->setText("Открытых окон: " + QString::number(graphYComp.last()));
        paintRastr2->update(); // Update painted rastr with new coordinates
    }
    else
    {
        paintRastr2->stepMov += 1;  // Representation of a Step
        errorHandling(drawGraph(ui->customPlot1)); // Draw line in graph 1
        ui->label->setText("Открытых окон: " + QString::number(graphY.last()));
        paintRastr2->update(); // Update painted rastr with new coordinates
    }
    return;
}

void MainWindow::drawThread(int startStep, int endStep)
{
    for (int i = startStep; i< endStep * 2; i++)
    {
        emit stepSignal();
        Sleep(500);
    }
}

void MainWindow::drawThreadOsci(int startStep, int endStep)
{
    for (int i = startStep; i < endStep * 4; i++)
    {
        emit stepSignalOsci();
        Sleep(500);
    }
}

void MainWindow::on_pushButtonStep_clicked()
{
    // Automatic mode with oscillation
    if ((1 == rastrManipulation.oscillation)&&(1 == dialog->mode))
    {
        runThread = QtConcurrent::run(this, &this->drawThreadOsci,paintRastr2->stepMov, rastrManipulation.jRastr);
        return;
    }

    // Manual mode with oscillation
    if ((1 == rastrManipulation.oscillation)&&(0 == dialog->mode))
    {
        oneStepOsci();
        return;
    }

    // Automatic mode without oscillation
    if ((0 == rastrManipulation.oscillation)&&(1 == dialog->mode))
    {
        runThread = QtConcurrent::run(this, &this->drawThread,paintRastr2->stepMov, rastrManipulation.jRastr);
        return;
    }

    // Manual mode without oscillation
    if ((0 == rastrManipulation.oscillation)&&(0 == dialog->mode))
    {
        oneStep();
        return;
    }
}

// Quits the program
void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_pushButtonColor_clicked()
{
    QColor checkColor = Qt::white;
    if (checkColor == paintRastr1->getBGColor())
    {
        paintRastr1->setBGColor(Qt::black);
        paintRastr1->setRastrColor(Qt::black,Qt::red);
    }
    else
    {
        paintRastr1->setBGColor(Qt::white);
        paintRastr1->setRastrColor(Qt::gray,Qt::white);
    }

    paintRastr1->update();
    paintRastr2->update();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "О программе",
                       "Функциональная библиотека и модуль визуализации для построения "
                       "\nи контроля корректности автокорреляционных растровых структур. "
                       "\nВерсия PreRelease v1.0"
                       "\n"
                       "\nПротопопов Михаил, ВКИ НГУ 2016");
}

void MainWindow::on_actionManual_triggered()
{
    helpWindow->show();
}

// Triggers saving feature
void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить"),
                                                    "", tr("Растровый файл (*.rastr);;Все файлы(*)")); // Call for an "save" window
    if (fileName.isEmpty())
        return;

    errorHandling(rastrManipulation.saveRastr(fileName));
}

// Triggers loading of a rastr from binary file
void MainWindow::on_actionLoad_triggered()
{
    if (rastrManipulation.rastr1 != NULL)
        checkForSave();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Загрузить"),
                                                    "", tr("Растровый файл (*.rastr);")); // Call for an "import" window

    if (fileName.isEmpty())
        return;

     errorHandling(rastrManipulation.loadRastr(fileName));

     paintRastr2->hide();

     if (( (this->minimumHeight() - 20) / 2.1) / rastrManipulation.iRastr < 4)
     {
         QMessageBox::information(this, tr("Ошибка"), tr("Слишком большая размерность матрицы."));
         rastrManipulation.deleteArray(rastrManipulation.iRastr);
         return;
     }

     paintRastr1->setParameters(ui->graphicsView_1->height(), ui->graphicsView_1->width(),
                                rastrManipulation.iRastr, rastrManipulation.jRastr,rastrManipulation.jRastr, Qt::gray,
                                rastrManipulation.iRastr, rastrManipulation.jRastr, 0); // Set parameters for background rastr
     paintRastr1->setBGColor(Qt::white);


     paintRastr1->setRastr(rastrManipulation.rastr1); // Sets matrix for background rastr

     ui->pushButtonStep->setEnabled(0); // Disables "Step" button
     ui->pushButtonStart->setEnabled(1); // Enables "Start" button
     ui->pushButtonColor->setEnabled(1);

     clearVectors();

     // Set tick length for first graph
     ui->customPlot1->xAxis->setAutoTickStep(false);
     ui->customPlot1->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
     ui->customPlot1->yAxis->setAutoTickStep(false);

     // Set tick length for second graph
     ui->customPlot2->xAxis->setAutoTickStep(false);
     ui->customPlot2->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
     ui->customPlot2->yAxis->setAutoTickStep(false);

     // Set tick length for third graph
     ui->customPlot3->xAxis->setAutoTickStep(false);
     ui->customPlot3->xAxis->setTickStep(rastrManipulation.jRastr*2 / 6);
     ui->customPlot3->yAxis->setAutoTickStep(false);

     // give the axes some labels:
     ui->customPlot1->xAxis->setLabel("AcF");
     ui->customPlot1->yAxis->setLabel("Количество открытых окон");
     // set axes ranges, so we see all data:
     ui->customPlot1->xAxis->setRange(0, rastrManipulation.jRastr*2);

     ui->customPlot2->xAxis->setLabel("AcF'");
     ui->customPlot2->yAxis->setLabel("Количество открытых окон");
     // set axes ranges, so we see all data:
     ui->customPlot2->xAxis->setRange(0, rastrManipulation.jRastr*2);

     ui->customPlot3->xAxis->setLabel("AcF' - AcF");
     ui->customPlot3->yAxis->setLabel("Количество открытых окон");
     // set axes ranges, so we see all data:
     ui->customPlot3->xAxis->setRange(0, rastrManipulation.jRastr*2);

     ui->actionExport->setEnabled(1);
     ui->actionSave->setEnabled(1);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (NULL != rastrManipulation.rastr1)
    {
        QMessageBox reply(QMessageBox::Question, tr("Несохраненные данные"), tr("Сохранить текущую матрицу?"),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
        reply.setButtonText(QMessageBox::Yes, tr("Да"));
        reply.setButtonText(QMessageBox::No, tr("Нет"));
        reply.setButtonText(QMessageBox::Cancel, tr("Отмена"));

        reply.exec();

        if (reply.result() == QMessageBox::Yes)
        {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Экспортировать"),
                                                             "", tr("Текстовый файл (*.txt);;Все файлы(*)")); // Call for an "export" window
            if (fileName.isEmpty())
                 return;

            errorHandling(rastrManipulation.saveRastr(fileName));
        }

        if (reply.result() == QMessageBox::No)
        {
            return;
        }

        if (reply.result() == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
        rastrManipulation.deleteArray(rastrManipulation.iRastr);
    }
}
