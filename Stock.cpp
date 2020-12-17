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
            this->url = "https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY_ADJUSTED&symbol=" + symbol + "&outputsize=full&datatype=csv";
        }
        
        ~StockData() {
            //delete [] easyhandle; not defined?    
            delete [] headers;
        }
        
        void setAPIKey(string key) {
            this->key = key;
        }
        
        void populateData() {
            initializeCurl();
            setHeaders();
            readData();
        }
        
        StockMap getData() {
            return data;
        }
        
        float getNDayAverage(int n) {
            float sum = 0;
            int i = 0;
            for (auto const& x : data) {
                if (i++ >= n) break;
                sum += x.second.close;
                //cout << x.second.close << endl;
            }
            return sum / (i-1);
        }
       
       
        string getLastDate() {
            for (auto const& x : data) return x.first;
        }
    
    private:
        string symbol;
        string url;
        string key;
        CURL * easyhandle;
        string readBuffer;
        const string expected_return = "timestamp,open,high,low,close,adjusted_close,volume,dividend_amount,split_coefficient"; 
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
            parseCSV();
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
            //cout << k << endl;
            if (1 != ((string)k).compare(expected_return)) {
                cout << "API Responded: " << k << endl;
                return;
            }
            
            for (k = strtok(NULL, "\n"); k != NULL; k = strtok(NULL, "\n")) { 
                unpacked.push_back(k);
                //data_size+=1;
            }
            
            for (string line : unpacked) {
                StockDataEntry temp = parseLine(line);
                data[temp.date] = temp;
                //cout << data[temp.date].date << endl;
            }
        }

};

int main(int argc, char * argv[]) {
    StockData Microsoft("MSFT");
    Microsoft.setAPIKey("ff4b7dad06mshe8f6632474c0fa5p14aba0jsn5304c3e949f5");
    Microsoft.populateData();
    StockData::StockMap MicrosoftData = Microsoft.getData();
    //cout << MicrosoftData["2020-07-31"].close << endl;
    cout << "For " << Microsoft.getLastDate() << endl;
    cout << "The 10 day moving average was " << Microsoft.getNDayAverage(10) << endl;
    cout << "The 100 day moving average was " <<  Microsoft.getNDayAverage(100) << endl;
    cout << "The 200 day moving average was " <<  Microsoft.getNDayAverage(200) << endl;
    cout << "The 300 day moving average was " <<  Microsoft.getNDayAverage(200) << endl;
    // TODO Vector for dates to preserve order!!!
    return 0;
}
