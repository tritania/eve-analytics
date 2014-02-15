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
#include "storage.h"
#include "fileio.h"
#include "download.h"
#include "economic.h"
#include <iostream>
using namespace std;

    string url;
    string user;
    string password;
    string database;

int main ()
{
    
    getSQLData();
    
    cout << "Starting" << endl;
    download();
    extract();
    strip();
    storedata();
    removelowsec();
    removenonman();
    
    
    return 0;
}
