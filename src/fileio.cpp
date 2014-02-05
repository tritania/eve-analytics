/*
 * Copyright 2014 <erikwilson@magnorum.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "fileio.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
using namespace std;

void strip()
{
    cout << "Stripping data file" << endl;
    ifstream file("data/latest.dump");
    ofstream fileout;
    fileout.open("data/latest.csv");
    string str; 
    while (getline(file, str))
    {
    for(int i=0; i<str.length(); i++)
        if(str[i] == '"') str.erase(i,1);
    for(int i=0; i<str.length(); i++)
        if(str[i] == ' ') str.erase(i,1);
    fileout << str << endl;
    }
    fileout.close();
}

void extract()
{
    cout << "Extracting data dump" << endl;
    string command = "gzip -d data/latest.dump.gz";
    system((const char*)command.c_str());
}
