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
#include <stdlib.h>
#include <iostream>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "storage.h"
using namespace std;

void storedata()
{
    cout << "Storing data in SQL" << endl;
    try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "password");
    /* Connect to the MySQL test database */
    con->setSchema("EVE_DATA");

    stmt = con->createStatement();
    stmt->execute("DROP TABLE IF EXISTS temp");
    stmt->execute("CREATE TABLE temp (orderid text, regionid text, systemid text, stationid text, typeid text, bid text, price text, minvolume text, volremain text, volenter text, issued text, duration text, ranges text, reportedby text, reportedtime text)");
    stmt->execute("LOAD DATA LOCAL INFILE '/home/tritania/Development/Eve-Manufacturing/data/latest.csv' INTO TABLE temp FIELDS TERMINATED BY ',' LINES TERMINATED BY '\r\n' IGNORE 1 LINES");
    stmt->execute("ALTER TABLE temp DROP minvolume, DROP ranges, DROP reportedtime, DROP duration, DROP reportedby");
    stmt->execute("DELETE FROM temp WHERE bid='0'");
    stmt->execute("ALTER TABLE temp DROP bid");
    delete stmt;
    delete con; 
    }
    
    catch (sql::SQLException &e){
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}

void removelowsec(); 
{
     cout << "Removing low-sec systems from table" << endl;
    try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "password");
    /* Connect to the MySQL test database */
    con->setSchema("EVE_DATA");

    stmt = con->createStatement();
    stmt->execute("DELETE FROM temp WHERE systemid = ANY(SELECT solarSystemID FROM mapsolarsystems WHERE security <0.5)"); //needs to be writte
    delete stmt;
    delete con; 
    }
    
    catch (sql::SQLException &e){
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}

void removenonman();
{
     cout << "Removing items that cannot be made" << endl;
    try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "password");
    /* Connect to the MySQL test database */
    con->setSchema("EVE_DATA");

    stmt = con->createStatement();
    stmt->execute(""); //needs to be writte
    delete stmt;
    delete con; 
    }
    
    catch (sql::SQLException &e){
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}

