#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
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
        StockDataEntry() {
            this->date = "-1"; this->open = -1; this->high = -1; this->low = -1;
            this->close = -1; this->adjusted_close = -1; this->volume = -1;
            this->dividend = -1; this->split_coefficient = -1;
        }
        ~StockDataEntry() { } 
        StockDataEntry(string date, float open, float high, float low, float close,
                       float adjusted_close, float volume, float dividend, float split_coefficient) {
            this->date = date; this->open = open; this->high = high; this->low = low;
            this->close = close; this->adjusted_close = adjusted_close; this->volume = volume;
            this->dividend = dividend; this->split_coefficient = split_coefficient;
        }
    };
    
    public:
        typedef map<string, StockDataEntry> StockMap;
        
        StockData(string symbol) {
            this->symbol = symbol;
            updateUrl();
        }
        
        ~StockData() {  
            delete [] headers;
        }
        
        //set api key to user defined key
        void setAPIKey(string key) {
            this->key = key;
        }
        
        //update outputsize to new size (full or compact)
        void setOutputSize(string outputsize) {
            this->outputsize = outputsize;
            updateUrl();
        }
        
        //query the api and set internal variables to api output
        void populateData() {
            initializeCurl();
            setHeaders();
            readData();
            parseCSV();
        }
        
        //get stock data as a StockMap (map date->stockdataentry)
        StockMap getData() { return data; }
        
        string getSymbol() { return symbol; }
        
        //function to return the average closing price over the last n-days
        float getNDayAverage(int n) {
            float sum = 0;
            StockMap::reverse_iterator it;
            int i = 0;
            for (it = data.rbegin(); it != data.rend(); it++) {
                if (i++ > n) break;
                sum += it->second.close;
            }
            return sum / (i);
        }
       
       // function to return last trading day (returns todays date if market is open)
        string getLastDate() {
            StockMap::reverse_iterator it;
            for (it = data.rbegin(); it != data.rend(); it++) 
                return it->first;
        }
    
    private:
        const string expected_return = "timestamp,open,high,low,close,adjusted_close,volume,dividend_amount,split_coefficient";
        string symbol;
        string url;
        string key;
        string readBuffer;
        string outputsize = "compact";
        
        CURL * easyhandle; 
        StockMap data;
        
        struct curl_slist * headers; //don't put variables below this (causes seg fault????!??!)
        
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
        
        void readData() {
            curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_perform(easyhandle);
        }
        
        void updateUrl() {
            url = "https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY_ADJUSTED";
            url += "&symbol=" + symbol;
            url += "&outputsize=" + outputsize;
            url += "&datatype=csv";
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
            return temp;
        }
        
        void parseCSV() {
            vector<string> unpacked;
            char * k = strtok((char*)readBuffer.c_str(), "\n");
            if (1 != ((string)k).compare(expected_return)) {
                cerr << "API Responded: " << k << endl;
                return;
            }
            
            for (k = strtok(NULL, "\n"); k != NULL; k = strtok(NULL, "\n")) 
                unpacked.push_back(k);
            
            for (string line : unpacked) {
                StockDataEntry temp = parseLine(line);
                data[temp.date] = temp;
            }
        }

};

int main(int argc, char * argv[]) {
    StockData Microsoft("MSFT");
    Microsoft.setAPIKey("ff4b7dad06mshe8f6632474c0fa5p14aba0jsn5304c3e949f5");
    Microsoft.setOutputSize("compact");
    Microsoft.populateData();
    
    StockData::StockMap MicrosoftData = Microsoft.getData();
    cout << "On 2020-12-17, Microsoft opened at ";
    cout << MicrosoftData["2020-12-17"].open << " and closed at ";
    cout << MicrosoftData["2020-12-17"].close << endl;
    cout << endl;
    
    cout << "For " << Microsoft.getSymbol() << " on " << Microsoft.getLastDate() << endl;
    cout << "The 10 day moving average was " << Microsoft.getNDayAverage(10) << endl;
    cout << "The 100 day moving average was " <<  Microsoft.getNDayAverage(100) << endl;
    //cout << "The 200 day moving average was " <<  Microsoft.getNDayAverage(200) << endl; only works with full api request
    cout << endl;
    
    return 0;
}
