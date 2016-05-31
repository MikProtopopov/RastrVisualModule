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

#ifndef RASTRMANIPULATION_H
#define RASTRMANIPULATION_H

#include <QString>
#include <stdint.h>

class RastrManipulation
{
public:
    RastrManipulation();
    int importRastr(QString fileName); // Function of importing rastrs
    int exportRastr(QString fileName); // Function of exporting rastrs
    int saveRastr(QString fileName);   // Function of saving rastrs
    char loadRastr(QString fileName);   // Function of loading rastrs
    void deleteArray(int DeleteLines); // Delete craeted rastrs
    int jacobi(int a, int p);

    int compareRastr(int stepHorisontal, int stepVertical); // Function for counting the amount of open windows in rastr overlap

    int fillRastr2();              // Function for filling moving rastr with consideration of "oscillation" value
    int countWindows();            // Function for counting open windows in rastr overlap
    void setOscillation(int osci); // Function for setting oscillation value

    bool oscillation; // Oscillation on/off (1/0)
    int iRastr;       // Number of lines in Rastr
    int jRastr;       // Number of elements in lines of Rastr
    uint8_t **rastr1;     // Main, background rastr
    uint8_t **rastr2;     // Secondaty, moving rastr
    uint8_t *rastrBuffer;     // Secondaty, moving rastr

    void createNewRastr(const int &xInt, const int &yInt);
    void createNewRastrAdamar(const int &xInt);


};

#endif // RASTRMANIPULATION_H
