#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <vector>

#include <curl/curl.h>
#include <sqlite3.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	return 0;
}

bool getFile(string filename) {
    bool retval = true;
    string url = "https://eve-central.com/dumps/";
    string furl = url + filename;
    CURL *curl_handle = curl_easy_init();
    FILE *pagefile;
    
    curl_easy_setopt(curl_handle, CURLOPT_URL, furl.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	pagefile = fopen(filename.c_str(), "wb");
	
	if (pagefile) {
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
		curl_easy_perform(curl_handle);
		fclose(pagefile);
	} else {
		retval = false;
	}
	
	curl_easy_cleanup(curl_handle);

    return retval;
}

int extractFile(sqlite3 *db, string filename) {
	
	using namespace boost::iostreams;
	using namespace boost::algorithm;
	
	ifstream file(filename, ios_base::in | ios_base::binary);	
	
    filtering_istream in;
    
    in.push(gzip_decompressor());
    in.push(file);
    
    std::vector<std::string> tokens;
    int where =0;
    int rc;
    char *sql;
    string q;
    char *zErrMsg = 0;

    for (string line; getline(in, line);) {
		if (where != 0) { 
			line.erase(remove(line.begin(), line.end(), '"'), line.end());
			split(tokens, line, is_any_of(","));
			
			q = "INSERT INTO ECD(BIDID,REGIONID,SYSTEMID,STATIONID,TYPEID,BIDTYPE,PRICE,MINVOL,VOLREM,VOLENTER) "  \
				  "VALUES (" + tokens[0] + "," + tokens[1] + "," + tokens[2] + "," + tokens[3] + "," + tokens[4] + "," + \
				  tokens[5] + "," + tokens[6] + "," + tokens[7] + "," + tokens[8] + "," + tokens[9] + "); ";

			rc = sqlite3_exec(db, q.c_str(), callback, 0, &zErrMsg);
			if (rc) {
				cout << sqlite3_errmsg(db) << endl;
			}
		}
		where++;
	}
    
	return 1;
}

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    sqlite3 *db;
    int rc;
    string in;
    char *sql; //create table query
    char *zErrMsg = 0;

    rc = sqlite3_open("data.db", &db);

    if (rc) {
		cout << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Opened database successfully" << endl;
    }
    
	sql = "CREATE TABLE ECD("  \
	 "ID INTEGER PRIMARY KEY  AUTOINCREMENT," \
	 "BIDID        INT    NOT NULL," \
	 "REGIONID     INT    NOT NULL," \
	 "SYSTEMID     INT    NOT NULL," \
	 "STATIONID    INT    NOT NULL," \
	 "TYPEID       INT    NOT NULL," \
	 "BIDTYPE      INT    NOT NULL," \
	 "PRICE        REAL   NOT NULL," \
	 "MINVOL       INT    NOT NULL," \
	 "VOLREM       INT    NOT NULL," \
	 "VOLENTER     INT    NOT NULL);";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		cout << zErrMsg << endl;
		sqlite3_free(zErrMsg);
	}else{
		cout << "Table created." << endl;
	}

    cout << "Please enter a date to parse: ";
    getline(std::cin, in);
    cout << "Downloading" << endl;
    
    in = in + ".dump.gz";
    //getFile(in);
    extractFile(db, in);

	cout << "Finished Downloading" << endl;

    sqlite3_close(db); //clean up
    return 0;
}
