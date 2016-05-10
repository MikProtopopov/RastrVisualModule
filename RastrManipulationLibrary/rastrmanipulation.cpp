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

#include "rastrmanipulation.h"
#include <malloc.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QString>
#include <QIODevice>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>

RastrManipulation::RastrManipulation()
{
    rastr1 = NULL;
    rastr2 = NULL;
    oscillation = 1;
}

int RastrManipulation::checkForSave()
{
    // TO DO
    // call window asking if user wants to save the existing array
    // save or export?
    // if YES - do it - return 1
    // if NO - carry on - return 0
    return 0;
}

// Set oscillation value
void RastrManipulation::setOscillation(int osci)
{
    oscillation = osci;
}

// Create new rastr with accepted parameters
void RastrManipulation::createNewRastr(const int &xInt, const int &yInt)
{
    iRastr = xInt;
    jRastr = yInt;

    if ((rastr1)&&(checkForSave()))
        deleteArray(iRastr);

    rastr1 = new uint8_t*[iRastr];
    for (int i=0; i<iRastr; i++)
        rastr1[i] = new uint8_t[jRastr];

    for (int i=0;i<iRastr;i++)
        for(int j=0;j<jRastr;j++)
            rastr1[i][j] = rand() % 2;

}

void RastrManipulation::createNewRastrAdamar(const int &xInt)
{
    iRastr = xInt;
    jRastr = xInt;

    if ((rastr1)&&(checkForSave()))
        deleteArray(iRastr);

    rastr1 = new uint8_t*[iRastr];
    for (int i=0; i<iRastr; i++)
        rastr1[i] = new uint8_t[jRastr];

    for (int i=0;i<iRastr;i++)
        for(int j=0;j<jRastr;j++)
            rastr1[i][j] = 1;

    for (int i=2;i<iRastr;i++)
    {
        rastr1[i][i] = 0;
        int a = legendre(i-1,iRastr-1);
        for(int j=0;j<jRastr - i;j++)
        {
            if (1 == a)
                rastr1[j+1][j+i] = a;
                rastr1[j+i][j+1] = 0;
            if (-1 == a)
                rastr1[j+1][j+i] = a;
                rastr1[j+i][j+1] = 1;
        }
    }
}

int RastrManipulation::legendre(int a, int p)
{
    if (1 == a)
        return 1;
    if (0 == a % 2)
        return (legendre(a/2,p) * ((-1)^((p^2 - 1) / 8)));
    if (1 == a % 2)
        return (legendre(p%a,p) * ((-1)^((a-1)*(p-1) / 4)));
}

int RastrManipulation::jacobi(int a, int p)
{
    if (1 == a)
        return 1;
    if (0 == a % 2)
        return (legendre(a/2,p) * ((-1)^((p^2 - 1) / 8)));
    if (1 == a % 2)
        return (legendre(p%a,p) * ((-1)^((a-1)*(p-1) / 4)));
}

// Delete existing array
void RastrManipulation::deleteArray(int DeleteLines) // DeleteLines - number of lines in array we are deleting
{
    for (int i=0; i<DeleteLines; i++)
        delete[] rastr1[i];
    delete[] rastr1;
    delete[] rastr2;
    rastr1 = NULL;
    rastr2 = NULL;
}

// Import matrix from file
int RastrManipulation::importRastr(QString fileName)
{
    if (fileName.isEmpty())
        return 1;
    QFile inFile(fileName); //File for import

    if (!inFile.open(QIODevice::ReadOnly)) // Viability check - can program open file?
    {
        return 2;
    }

    if ((rastr1)&&(checkForSave())) // Delete array if exists
        deleteArray(iRastr);

    QString readBufferString;          // Read-string from file
    QStringList readBufferInt;         // String list for "conversion" to integer
    QTextStream inTextStream(&inFile); // Read stream of text

    int jRastrPrev = -1; //Number of elements in previous line
    iRastr = 0;
    jRastr = 0;

    // Counting the number of lines:
    while (!inTextStream.atEnd())
    {
        readBufferString = inTextStream.readLine();
        iRastr++;
    }

    //Checking validity of the number of lines in file
    if (iRastr < 2)
    {
        inFile.close();
        return 3;
    }

    //Creating new array
    rastr1 = new uint8_t*[iRastr];

    //Counting the number of elements in the lines, checking for validity
    inTextStream.seek(0);
    for (int i=0; i<iRastr; i++)
    {
        readBufferString = inTextStream.readLine();
        readBufferString.replace(QString(" "), QString(""));
        readBufferString.replace(QString("\n"), QString(""));
        readBufferString.replace(QString("\t"), QString(""));
        readBufferInt = readBufferString.split(",");

        jRastr = readBufferInt.count();

        if ((jRastrPrev != -1)&&(jRastrPrev != jRastr))
        {
            deleteArray(iRastr);
            jRastr = 0;
            inFile.close();
            return 4;
        }

        rastr1[i] = new uint8_t[jRastr];

        jRastrPrev = jRastr;

        //Writing elements into the matrix
        for (int j=0; j<jRastr; j++)
            if (readBufferInt.value(j) == "1")
                rastr1[i][j] = 1;
            else
                if (readBufferInt.value(j) == "0")
                    rastr1[i][j] = 0;
                else
                {
                    deleteArray(i);
                    inFile.close();
                    return 5;
                }
    }

    inFile.close();
    return 0;
}

// Export matrix into file
int RastrManipulation::exportRastr(QString fileName)
{
    if (fileName.isEmpty())
        return 1;
    else
    {
        QFile file(fileName); //File for export
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file); //Text stream for export
        for (int i=0; i<iRastr; i++){
            out << rastr1[i][0];
            for (int j=1; j<jRastr; j++)
                out << "," << rastr1[i][j];
            out << endl;
        }
        file.close();
    }
    return 0;
}

// Save rastr into a binary file
int RastrManipulation::saveRastr(QString fileName)
{
    if (fileName.isEmpty())
        return 1;
    else
    {
        QFile file(fileName); // File for export
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file); // Data stream for export
        out.writeRawData((char*)&iRastr,sizeof(iRastr));
        out.writeRawData((char*)&jRastr,sizeof(jRastr));

        for (int i=0; i<iRastr; i++)
            out.writeRawData((char*)rastr1[i], sizeof(uint8_t)*jRastr);
        file.close();
    }
    return 0;
}

char RastrManipulation::loadRastr(QString fileName)
{

    // TODO check for size - выписать размерности, перемножить, сравнить

    if (fileName.isEmpty())
        return 1;
    QFile inFile(fileName); //File for import

    if (!inFile.open(QIODevice::ReadOnly)) // Viability check - can program open file?
    {
        return 2;
    }

    if ((rastr1)&&(checkForSave())) // Delete array if exists
        deleteArray(iRastr);

    QDataStream inDataStream(&inFile); // Read stream of text

    inDataStream.readRawData((char*)&iRastr,sizeof(iRastr));
    inDataStream.readRawData((char*)&jRastr,sizeof(jRastr));

    //Checking validity of the file
    if (iRastr * jRastr < 9)
    {
        inFile.close();
        return 3;
    }

    rastrBuffer = new uint8_t[iRastr * jRastr];
    inDataStream.readRawData((char*)rastrBuffer,sizeof(uint8_t)* iRastr * jRastr);

    rastr1 = new uint8_t*[iRastr];

    for (int i=0; i<iRastr; i++)
    {
        rastr1[i] = new uint8_t[jRastr];
        memmove(rastr1[i],rastrBuffer + i*jRastr,jRastr);
    }
    inFile.close();
    delete rastrBuffer;
    return 0;
}

int RastrManipulation::fillRastr2()
{
        if (0 == oscillation)
            rastr2 = rastr1;
        if (1 == oscillation)
        {
            try {
            rastr2 = new uint8_t*[iRastr - 1];
            for (int i=0; i<iRastr - 1; i++)
                rastr2[i] = rastr1[i+1];
            } catch (...) {
                return 7;
            }
        }
        return 0;
}


// Calculate the number of open windows
int RastrManipulation::compareRastr(int stepHorisontal, int stepVertical)
{
    int countWindow = 0;
    int columns = jRastr - abs(stepHorisontal - jRastr); // Number of columns
    int rows = jRastr - abs(stepVertical - jRastr); // Number of rows

    if (0 == oscillation)
        for (int i=0; i<iRastr; i++)
            for (int j=0; j<columns; j++)
                countWindow += rastr2[i][jRastr - columns + j] * rastr1[i][j];

    if (1 == oscillation)
        for (int i=0; i<iRastr - 1; i++)
            for (int j=0; j<columns; j++)
            {
                if (stepHorisontal < jRastr)
                    countWindow += rastr2[i][jRastr - columns + j] * rastr1[i + stepVertical][j];
                else
                    countWindow += rastr2[i][j] * rastr1[i + stepVertical][jRastr - columns + j];
            }



    return countWindow;
}

int RastrManipulation::countWindows()
{
    if (NULL == rastr2)
        return 0;
    int countWindows = 0;

    for (int i=0; i<iRastr-oscillation; i++)
        for (int j=0; j<jRastr; j++)
            countWindows += rastr2[i][j];
    return countWindows;
}
