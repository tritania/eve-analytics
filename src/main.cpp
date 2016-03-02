#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>
#include <sqlite3.h>


static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

bool getFile(std::string filename) {
    bool retval = true;
    std::string url = "https://eve-central.com/dumps/";
    std::string furl = url + filename;
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

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    sqlite3 *db;
    int rc;
    std::string in;

    rc = sqlite3_open("data.db", &db);

    if (rc) {
		std::cout << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Opened database successfully" << std::endl;
    }

    std::cout << "Please enter a date to parse: ";
    std::getline(std::cin, in);
    std::cout << "Downloading" << std::endl;
    
    in = in + ".dump.gz";
    getFile(in);

	std::cout << "Finished Downloading" << std::endl;

    sqlite3_close(db); //clean up
    return 0;
}
