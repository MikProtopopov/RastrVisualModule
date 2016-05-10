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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "dialog.h"
#include "startwindow.h"
#include "rastrmanipulation.h"
#include "paintrastr.h"
#include "helpwindow.h"

#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void errorHandling(int);
    int drawGraph(QCustomPlot *customPlot);        // Draw non-oscillated graph
    int drawGraphOsci(QCustomPlot *customPlot);    // Draw graph for oscillation
    int drawGraphCompare(QCustomPlot *customPlot); // Draw graph for comparison
    void checkForSave();                           // Function that checks if user wants to save current matrix

    void resizeEvent(QResizeEvent * event);

    RastrManipulation rastrManipulation; // Exemplar of RastrManipulation class
    PaintRastr *paintRastr1;             // Exemplar of PaintRastr class for moving rastr
    PaintRastr *paintRastr2;             // Exemplar of PaintRastr class for not moving rastr

    QVector<double> graphX;     // Vector for drawing of graph, for X axis
    QVector<double> graphXOsci; // Vector for drawing of graph, for X axis, for oscillated graph
    QVector<double> graphXComp; // Vector for drawing of graph, for X axis, for comparison graph

    QVector<double> graphY;     // Vector for drawing of graph, for Y axis, for non-oscillated graph
    QVector<double> graphYOsci; // Vector for drawing of graph, for Y axis, for oscillated graph
    QVector<double> graphYComp; // Vector for drawing of graph, for Y axis, for comparison graph

private slots:
    void clearVectors();
    void on_pushButtonStart_clicked(); // Action trigger for starting rastr movement
    void on_actionExport_clicked();    // Action trigger for export
    void on_actionImport_clicked();    // Action trigger for import
    void on_actionNew_clicked();       // Action trigger for creating new rastr

    void on_pushButtonStep_clicked(); // Action trigger for rastr movement

    void on_actionQuit_triggered(); // Action trigger for exit

    void on_pushButtonColor_clicked(); // Action trigger for color change

    void on_actionAbout_triggered();  // Action trigger for About window
    void on_actionManual_triggered(); // Action trigger for User Manula window

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

private:
    Ui::MainWindow *ui;
    Dialog *dialog;
    StartWindow *sWindow;
    HelpWindow *helpWindow;
    void createAction();
    void createMenu();
    void newFile();
    void open();
    void save();
    void saveAs();
    void quit();

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *actionNew;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionLoad;
    QAction *actionQuit;
    QAction *exitAction;
    QAction *actionExport;
    QAction *actionImport;


    void on_actionSave_clicked();
};

#endif // MAINWINDOW_H
