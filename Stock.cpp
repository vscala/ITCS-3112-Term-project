#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <curl/curl.h>
#include <bits/stdc++.h> 
using namespace std;

class StockData {
    struct StockDataEntry {
        string date;
        float open;
        float high;
        float low;
        float close;
        float adjusted_close;
        float volume;
        float dividend;
        float split_coefficient;
        StockDataEntry() {}
        StockDataEntry(string date, float open, float high, float low, float close,
                       float adjusted_close, float volume, float dividend, float split_coefficient) {
            this->date = date; this->open = open; this->high = high; this->low = low;
            this->close = close; this->adjusted_close = adjusted_close; this->volume = volume;
            this->dividend = dividend; this->split_coefficient = split_coefficient;
        }
    };
    private:
        string symbol;
        string url;
        string key;
        CURL * easyhandle;
        struct curl_slist * headers;
        
        static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        }
        
        void initializeCurl() {
            easyhandle = curl_easy_init();
            curl_global_init(CURL_GLOBAL_ALL);
            curl_easy_setopt(easyhandle, CURLOPT_CUSTOMREQUEST, "GET");
            curl_easy_setopt(easyhandle, CURLOPT_URL, url.c_str());
        }
        
        void setHeaders() {
            string keyheader = "x-rapidapi-key: " + key;
            headers = curl_slist_append(headers, keyheader.c_str());
            headers = curl_slist_append(headers, "x-rapidapi-host: alpha-vantage.p.rapidapi.com");
            curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, headers);
        }
        
        string readData() {
            string readBuffer;
            curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_perform(easyhandle);
            return readBuffer;
        }
        
        StockDataEntry parseLine(string line) {
            vector <string> tokens; 
            stringstream check1(line); 
            string intermediate; 
            
            while(getline(check1, intermediate, ',')) { 
                tokens.push_back(intermediate); 
            } 
            
            StockDataEntry temp(tokens[0], atof(tokens[1].c_str()), atof(tokens[2].c_str()), 
                             atof(tokens[3].c_str()), atof(tokens[4].c_str()), 
                             atof(tokens[5].c_str()), atof(tokens[6].c_str()), 
                             atof(tokens[7].c_str()), atof(tokens[8].c_str()));
            cout << "Closing price: " << temp.close << " for " << temp.date << endl;
            return temp;
        }
        
        void parseData(string readBuffer) {
            //Unpack readBuffer
            vector<string> unpacked;
            char * k = strtok((char*)readBuffer.c_str(), "\n");
            for (k = strtok(NULL, "\n"); k != NULL; k = strtok(NULL, "\n")) { 
                unpacked.push_back(k);
            }
            
            //Parse unpacked buffer
            vector<StockDataEntry> entries;
            for (string line : unpacked) {
                entries.push_back(parseLine(line));
            }
        }
        

    public:
        StockData(string symbol) {
            this->symbol = symbol;
            this->url = "https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY_ADJUSTED&symbol=" + symbol + "&outputsize=compact&datatype=csv";
        }
        
        void setAPIKey(string key) {
            this->key = key;
        }
        
        void populateData() {
            initializeCurl();
            setHeaders();
            string readBuffer = readData();
            parseData(readBuffer);
        }
};

int main(int argc, char * argv[]) {
    StockData Microsoft("MSFT");
    Microsoft.setAPIKey("ff4b7dad06mshe8f6632474c0fa5p14aba0jsn5304c3e949f5");
    Microsoft.populateData();
    return 0;
}
