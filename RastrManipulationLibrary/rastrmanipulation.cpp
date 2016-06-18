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
#include <windows.h>

RastrManipulation::RastrManipulation()
{
    rastr1 = NULL;
    rastr2 = NULL;
    oscillation = 1;
}

// Set oscillation value
void RastrManipulation::setOscillation(int osci)
{
    oscillation = osci;
}

// Create new rastr with accepted parameters
int RastrManipulation::createNewRastr(const int &xInt, const int &yInt)
{
    if (NULL != rastr1)
        deleteArray(iRastr);

    iRastr = xInt;
    jRastr = yInt;

    rastr1 = new uint8_t*[iRastr];
    for (int i=0; i<iRastr; i++)
        rastr1[i] = new uint8_t[jRastr];

    for (int i=0;i<iRastr;i++)
        for(int j=0;j<jRastr;j++)
            rastr1[i][j] = rand() % 2;
    return 0;
}

void RastrManipulation::createNewRastrAdamar(const int &xInt)
{
    iRastr = xInt;
    jRastr = xInt;

    if (rastr1)
        deleteArray(iRastr);

    rastr1 = new uint8_t*[iRastr];
    for (int i=0; i<iRastr; i++)
        rastr1[i] = new uint8_t[jRastr];

    for (int i=0;i<iRastr;i++)
        for(int j=0;j<jRastr;j++)
           rastr1[i][j] = 1;     //!!!!

    for (int i=1;i<iRastr;i++) //was 2 korabel
    {
        rastr1[i][i] = 0;
        int a = jacobi(i,iRastr-1);
        for(int j=0;j<jRastr - i - 1;j++)
        {
            if (1 == a)
            {
                rastr1[j+1][j+i+1] = 1;
                rastr1[j+i+1][j+1] = 0;
            }
            else
            {
                rastr1[j+1][j+i+1] = 0;
                rastr1[j+i+1][j+1] = 1;
            }
        }
    }
}

int RastrManipulation::createNewRastrMurzina(const int &xInt)
{
    iRastr = xInt;
    jRastr = xInt;
    createNewRastrAdamar(iRastr);

    uint8_t **localRastr = new uint8_t*[2*iRastr];
    for (int i=0; i<2*iRastr; i++)
        localRastr[i] = new uint8_t[jRastr];

    for (int i=0; i<2*iRastr;i++)
        for (int j=0; j<jRastr; j++)
            if (0 == i % 2)
                localRastr[i][j] = !rastr1[i/2][j];
            else
                localRastr[i][j] = rastr1[i/2][j];

    for (int i=0; i<2*iRastr; i++)
        localRastr[i][0] = 1;

    uint8_t *arrayL = new uint8_t[jRastr];
    memset(arrayL, 1, jRastr);

    rastr1[0][1] = !rastr1[iRastr-1][jRastr-1];

    for (int j=2; j<jRastr; j++)
        arrayL[j] = !rastr1[iRastr-1][j-1];

    deleteArray(iRastr);

    rastr1 = new uint8_t*[2*iRastr + 1];
    for (int i=0; i<2*iRastr + 1; i++)
        rastr1[i] = new uint8_t[jRastr];

    for (int i=0; i<2*iRastr; i++)
        memmove(rastr1[i],localRastr[i],jRastr);

    memmove(rastr1[2*iRastr],arrayL, jRastr);

    int n0 = jRastr/2 + 1;

    for (int j=n0; j < jRastr; j+=2)
    {
        for (int i=0; i < iRastr*2 + 1; i++)
            rastr1[i][j] = !rastr1[i][j];

        for (int i=0; i < iRastr*2; i++)
            localRastr[i][j] = !localRastr[i][j];
    }

    for (int i=0; i<iRastr*2 + 1; i++)
    {
        rastr1[i][1] = 1;
        rastr1[i][jRastr-1] = 1;
    }

    for (int i=0; i<iRastr*2; i++)
    {
        localRastr[i][1] = 1;
        localRastr[i][jRastr-1] = 1;
    }

    uint8_t **rastrA2 = new uint8_t*[2];
    for (int i=0; i<2; i++)
        rastrA2[i] = new uint8_t[jRastr];

    memmove(rastrA2[0],rastr1[2],jRastr);
    memmove(rastrA2[1],rastr1[0],jRastr);

    uint8_t **tempRastr = new uint8_t*[2*iRastr+3];
    for (int i=0; i<2*iRastr+3; i++)
        tempRastr[i] = new uint8_t[jRastr];

    memmove(tempRastr[0],rastrA2[0],jRastr);
    memmove(tempRastr[1],rastrA2[1],jRastr);

    for (int i=2; i<iRastr*2 + 3; i++)
        memmove(tempRastr[i],rastr1[i-2],jRastr);

    rastr2 = new uint8_t*[2*iRastr+2];
    for (int i=0; i<2*iRastr; i++)
    {
        rastr2[i] = new uint8_t[jRastr];
        memmove(rastr2[i],localRastr[2*iRastr-1-i],jRastr);
    }

    memmove(rastr2[2*iRastr+1],rastr1[1],jRastr);
    memset(rastr2[2*iRastr],1,jRastr);

    for (int i=0; i<2*iRastr; i++)
        delete localRastr[i];
    delete localRastr;

    localRastr = new uint8_t*[2*iRastr+2];
    for (int i=0; i<2*iRastr+2;i++)
    {
        localRastr[i] = new uint8_t[jRastr];
        memmove(localRastr[i],rastr2[i],jRastr);
        delete[] rastr2[i];
    }

    delete rastr1;
    delete rastr2;

    iRastr = 2*iRastr + 3;
    jRastr = 2*jRastr;

    rastr1 = new uint8_t*[iRastr];
    for (int i=0; i<iRastr; i++)
    {
        rastr1[iRastr - 1 - i] = new uint8_t[jRastr];
        for (int j=0; j<jRastr; j++)
            rastr1[iRastr - 1 - i][j] = tempRastr[i][j/2];
    }

    rastr2 = new uint8_t*[iRastr-1];
    for (int i=0;i<iRastr-1;i++)
    {
        rastr2[i] = new uint8_t[jRastr];
        for (int j=0; j<jRastr; j++)
            rastr2[i][j] = localRastr[i][j/2];
    }



// -----/!\-----/!\-----
//    deleteArray(iRastr);

//    rastr1 = new uint8_t*[2*iRastr];
//    for (int i=0; i<2*iRastr; i++)
//        rastr1[i] = new uint8_t[jRastr];

//    for (int i=0;i<2*iRastr;i++)
//        memmove(rastr1[i],localRastr[i],jRastr);
    // -----/!\-----/!\-----

    for (int i=0; i<((iRastr-3)/2); i++)
        delete localRastr[i];
    for (int i=0; i<iRastr; i++)
        delete tempRastr[i];
    delete tempRastr;
    delete rastrA2[0];
    delete rastrA2[1];
    delete rastrA2;
    delete localRastr;
    delete arrayL;

    return 0;
}

int RastrManipulation::jacobi(int a, int p)
{
    int s;
        long a1 = a, e = 0, m, p1;

        if (1 == a)
            return 1;
        if (0 == a % p)
            return 0;
        while (0 == (a1 & 1))
        {
          a1 >>= 1;
          e++;
        }

        m = p % 8;
        if (!(e & 1) || 1 == m || 7 == m)
            s = 1;
        else
            if (m == 3 || m == 5)
                s = - 1;

        if (p % 4 == 3 && a1 % 4 == 3)
            s = - s;

        if (a1 != 1)
            p1 = p % a1;
        else
            p1 = 1;

      return s * jacobi(p1, a1);
}

// Delete existing array
void RastrManipulation::deleteArray(int DeleteLines) // DeleteLines - number of lines in array we are deleting
{
    for (int i=0; i<DeleteLines; i++)
    {
        delete[] rastr1[i];
        rastr1[i] = NULL;

        if(NULL != rastr2)
           delete [] rastr2[i];
    }
    delete[] rastr1;
    rastr1 = NULL;

    if ((rastr2 != NULL)&&(1 == oscillation))
        delete[] rastr2;

    rastr2 = NULL;
}

// Import matrix from file
int RastrManipulation::importRastr(QString fileName)
{
    if (fileName.isEmpty())
        return 1;
    QFile inFile(fileName); //File for import

    if (!fileName.contains(".txt"))
        return 2;

    if (!inFile.open(QIODevice::ReadOnly)) // Viability check - can program open file?
    {
        return 2;
    }

    if (rastr1) // Delete array if exists
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
        if (!readBufferString.isEmpty())
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

        if (!readBufferString.isEmpty())
        {
            readBufferString.replace(QString(" "), QString(""));
            readBufferString.replace(QString("\n"), QString(""));
            readBufferString.replace(QString("\t"), QString(""));

        readBufferInt = readBufferString.split(",");

        jRastr = readBufferInt.count();

        if ((jRastrPrev != -1)&&(jRastrPrev != jRastr))
        {
            deleteArray(i-1);
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

    if (!fileName.contains(".rastr"))
        return 2;

    QFile inFile(fileName); //File for import

    if (!inFile.open(QIODevice::ReadOnly)) // Viability check - can program open file?
    {
        return 2;
    }

    if (rastr1) // Delete array if exists
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

bool RastrManipulation::compareShlishevsky(int maxCount)
{
    if (maxCount < compareRastr(jRastr,1))
        return false;

    for (int i=1; i<jRastr; i++)
        if (0 != compareRastr(jRastr + i, 1) - compareRastr(jRastr + i, 0))
            return false;

    for (int i=1; i<jRastr; i++)
        if (0 != compareRastr(i, 1) - compareRastr(i, 0))
            return false;
    return true;
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
