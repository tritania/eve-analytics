#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

#include <curl/curl.h>
#include <sqlite3.h>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using namespace std;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
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

int extractFile(string filename) {
	
	using namespace boost::iostreams;
	
	ifstream file(filename, ios_base::in | ios_base::binary);
	ofstream dfile("dump.csv");
	
    filtering_streambuf<input> in;
    filtering_streambuf<output> out;
    
    in.push(gzip_decompressor());
    in.push(file);
    
    out.push(dfile);
    
    boost::iostreams::copy(in, out);
    
	return 1;
}

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    sqlite3 *db;
    int rc;
    string in;

    rc = sqlite3_open("data.db", &db);

    if (rc) {
		cout << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Opened database successfully" << endl;
    }

    cout << "Please enter a date to parse: ";
    getline(std::cin, in);
    cout << "Downloading" << endl;
    
    in = in + ".dump.gz";
    //getFile(in);
    extractFile(in);

	cout << "Finished Downloading" << endl;

    sqlite3_close(db); //clean up
    return 0;
}
