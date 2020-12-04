#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

using namespace std;

void readStockData(string symbol) {
    CURL *hnd = curl_easy_init();
    FILE * file;
    file = fopen("out.dat", "w");
    
    //string temp = "https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY&symbol=" + symbol + "&outputsize=full&datatype=json";
    
    //"https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY&symbol=fdsfsdfa&outputsize=full&datatype=json"
    
    //cout << temp << endl;
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY&symbol=TESLA&outputsize=full&datatype=json");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "x-rapidapi-key: ff4b7dad06mshe8f6632474c0fa5p14aba0jsn5304c3e949f5");
    headers = curl_slist_append(headers, "x-rapidapi-host: alpha-vantage.p.rapidapi.com");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, file);

    CURLcode ret = curl_easy_perform(hnd);
}

int main() {
    string symbol = "TESLA";
	readStockData(symbol);
	return 0;
}
