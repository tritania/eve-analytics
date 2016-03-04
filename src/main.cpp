#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>

#include <curl/curl.h>
#include <sqlite3.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
using namespace boost::iostreams;
using namespace boost::algorithm;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

bool file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    return 0;
}

bool getDatabase() {
    bool retval = true;
    string filename = "eve.db.bz2";
    string filename2 = "eve.db";
    string url = "https://www.fuzzwork.co.uk/dump/sqlite-latest.sqlite.bz2";
    CURL *curl_handle = curl_easy_init();
    FILE *pagefile;

    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
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

    ifstream file(filename, ios_base::in | ios_base::binary);
    ofstream filed(filename2);

    filtering_streambuf<input> in;
    filtering_streambuf<output> out;

    in.push(bzip2_decompressor());
    in.push(file);

    out.push(filed);

    boost::iostreams::copy(in, out);
    remove(filename.c_str());

    return retval;
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
    sqlite3_stmt *stmt;
    char *zErrMsg = 0;
    char sSQL [256] = "\0";

    sprintf(sSQL, "INSERT INTO ECD VALUES (NULL,@ONE,@TWO,@THR,@FOR,@FIV,@SIX,@SVN,@EIG,@NIN,@TEN)");
    rc = sqlite3_prepare_v2(db,  sSQL, 256, &stmt, NULL);
    if (rc) {
        cout << sqlite3_errmsg(db) << endl;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
    if (rc) {
        cout << sqlite3_errmsg(db) << endl;
    }

    for (string line; getline(in, line);) {
        if (where != 0) {
            line.erase(remove(line.begin(), line.end(), '"'), line.end());
            split(tokens, line, is_any_of(","));

            sqlite3_bind_text(stmt, 1, tokens[0].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, tokens[1].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, tokens[2].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, tokens[3].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 5, tokens[4].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 6, tokens[5].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 7, tokens[6].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 8, tokens[7].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 9, tokens[8].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 10, tokens[9].c_str(), -1, SQLITE_TRANSIENT);

            sqlite3_step(stmt);
            sqlite3_reset(stmt);
        }
        where++;
    }

    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
    sqlite3_finalize(stmt);

    remove(filename.c_str());

    return 1;
}

bool checkDate(sqlite3 *db, string date) {
    string sSQL = "SELECT * FROM ADATES WHERE DAY = '" + date + "';";
    sqlite3_stmt* stmt;
    const unsigned char * text;
    char *zErrMsg = 0;
	int res;
	bool ret = false;

    sqlite3_prepare_v2(db,  sSQL.c_str(), strlen(sSQL.c_str()) + 1, &stmt, NULL);
	res = sqlite3_step(stmt);
	if (res == SQLITE_ROW) {
		ret = true;
	} else if (res == SQLITE_DONE) {
		ret = false;
	} else {
		cout << sqlite3_errmsg(db) << endl;
	}
	//text  = sqlite3_column_text (stmt, 1);
	sqlite3_finalize(stmt);
	
	return ret;
}

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    sqlite3 *db;
    int rc;
    string in;
    char *sql, *sql2;
    string sql3;
    char *zErrMsg = 0;
    
    if (!file_exist("eve.db")) {
		cout << "Getting eve database." << endl;
		getDatabase();
	}

    rc = sqlite3_open("eve.db", &db);

    if (rc) {
        cout << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Opened database successfully" << endl;
        sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
        sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);
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
     
    sql2 = "CREATE TABLE ADATES("  \
     "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
     "DAY TEXT NOT NULL);";

    sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    sqlite3_exec(db, sql2, callback, 0, &zErrMsg);

    cout << "Please enter a date to parse: ";
    getline(std::cin, in);
    checkDate(db, in);
    if (!checkDate(db, in)) {
		in = in + ".dump.gz";
		cout << "Downloading" << endl;
		getFile(in);
		cout << "Database insertion" << endl;
		extractFile(db, in);
	} else {
		cout << "That date has already been added." << endl;
	}

    cout << "Finished." << endl;

    sqlite3_close(db); //clean up
    return 0;
}
