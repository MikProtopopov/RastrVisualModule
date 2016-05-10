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

#ifndef PAINTRASTR_H
#define PAINTRASTR_H

#include <QWidget>
#include <stdint.h>

class PaintRastr : public QWidget
{
    Q_OBJECT
public:
    explicit PaintRastr(QWidget *parent = 0);


    void setParameters(int height, int width, int axisX, int axisY, int step, QColor color,
                       int staticAxisX, int staticAxisY, int typeOfRastr); // Set parameters of graphs

    void setSize(int height, int width); // Set size of the draw area

    int ProcessX(int i); // Change virtual X coordinate into actual one
    int ProcessY(int j); // Change virtual Y coordinate into actual one

    void setRastr(uint8_t **&localRastr); // Set rastr, which will be painted by the widget
    void setBGColor(QColor color);    // Changes background color of draw area
    QColor getBGColor();              // Returns current background color of draw area

    void setRastrColor(QColor color0, QColor color1); // Changes color of background rastr

    uint8_t **rastr; // Rastr for painting
    int stepMov; // Step of movement
    int oStatus; // Counts the amount of vertical changes during oscillation


protected:
    void paintEvent(QPaintEvent *);

private:
    int elemCountX; // Amount of elements on one side of rastr
    int elemCountY; // Amount of elements on one side of rastr
    int cellHeight; // Height of one cell in rastr
    int cellWidth;  // Width of one cell
    int staticX;    // "Static" coordinate for X, for painting rastr while oscillation is on
    int staticY;    // "Static" coordinate for Y, for painting rastr while oscillation is on
    int rastrType;  // Type of rastr - oscillated or non-oscillated
    int sizeWidth;  // Width of the draw area
    int sizeHeight; // Height of the draw area

    QColor rastrColor; // Value for color, to distinguish between stationary and moving rastrs
    QColor bgColor;    // Value for color of the background for stationary rastr
    QColor cellColor;  // Color of cells of rastr


signals:

public slots:
};

#endif // PAINTRASTR_H
