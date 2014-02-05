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
#include "download.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#include <sstream>
using namespace std;

void download()
{
    cout << "Starting Download of the Latest Data" << endl;
    string WEBSITE = "https://eve-central.com/dumps/";
    char mbstr[23];
    time_t now = time(NULL);
    now = now - (24*60*60);
    struct tm *t = localtime(&now);
    sprintf(mbstr,"20%02d-%02d-%02d", t->tm_year - 100, t->tm_mon+1, t->tm_mday);
    stringstream ss;
    string date;
    ss<<mbstr;
    ss>>date;
    string command = "wget -O data/latest.dump.gz " + WEBSITE + date + ".dump.gz";
    system((const char*)command.c_str());
}
