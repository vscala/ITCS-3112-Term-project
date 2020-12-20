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
        
        StockDataEntry(string date, float open, float high, float low, float close,
                       float adjusted_close, float volume, float dividend, float split_coefficient) {
            this->date = date; this->open = open; this->high = high; this->low = low;
            this->close = close; this->adjusted_close = adjusted_close; this->volume = volume;
            this->dividend = dividend; this->split_coefficient = split_coefficient;
        }
        
        ~StockDataEntry() { } 
    };
    
    public:
        typedef map<string, StockDataEntry> StockMap;

        /**
         *  Constructor that takes a symbol to use in this object
         *  @param symbol Symbol as a string used in this object
         */
        StockData(string symbol) {
            this->symbol = symbol;
            updateUrl();
        }

        /**
         *  Destructor to free headers
         */
        ~StockData() {  
            delete [] headers;
        }

        /**
         *  Method to set api key to user defined key
         *  @param key the new API key
         */
        void setAPIKey(string key) {
            this->key = key;
        }

        /**
         *  Method to update outputsize to new size (full or compact)
         *  @param outputsize string containing the new output size
         */
        void setOutputSize(string outputsize) {
            this->outputsize = outputsize;
            updateUrl();
        }

        /**
         *  Method to query the api and set internal variables to api output
         */
        void populateData() {
            initializeCurl();
            setHeaders();
            readData();
            parseCSV();
        }

        /**
         *  Method to get stock data as a StockMap (map: date->stockdataentry)
         *  @returns StockMap of all data entries
         */
        StockMap getData() { return data; }

        /**
         *  Method to return date vector
         *  @returns Date vector
         */
        vector<string> getDateVector() { return datev; }

        /**
         *  Method to return stock symbol
         *  @returns Stock symbol
         */
        string getSymbol() { return symbol; }

        /**
         *  Method to return the total count of data entries
         *  @returns Count of data entries
         */
        int getLength() { return datev.size(); }

        /**
         *  Method to return the most recent date in the date vector
         *  @returns The most recent date in the date vector
         */
        string getFirstDate() { return datev[getLength()-1]; }

        /**
         *  Method to return the oldest date in the date vector
         *  @returns The oldest date in the date vector
         */
        string getLastDate() { return datev[0]; }

        /**
         *  Method for returning the nth date of the date vector
         *  @param n The index of the date to be returned
         *  @returns The nth date in the date vector
         */
        string getNthDate(int n) { return datev[n]; }

        /**
         *  Method for returning the N-Day Moving Average 
         *  @param n The number of days to include in the average
         *  @param start The starting date of the moving average as an index of the date vector
         *  @returns The N-Day Moving Average
         */
        float getNDayAverage(int n, int start = 0) {
            float sum = 0; //needs additional testing
            int i;
            for (i = start; i < n+start; i++) {
                if (i >= datev.size()) { i--; break; }
                sum += data[datev[i]].close;
            }
            return sum / (i+1);
        }

        /**
         *  Method for returning this class as a string
         *  @returns a string containing all data in StockMap
         */
        string to_string() {
            string out = "";
            for (string date : datev) {
                out += date;
                out += "\n\t open: " + std::to_string(data[date].open);
                out += "\n\t close: " + std::to_string(data[date].close);
                out += "\n\t adjusted_close: " + std::to_string(data[date].adjusted_close);
                out += "\n\t low: " + std::to_string(data[date].low);
                out += "\n\t high: " + std::to_string(data[date].high);
                out += "\n\t volume: " + std::to_string(data[date].volume);
                out += "\n\t dividend: " + std::to_string(data[date].dividend);
                out += "\n\t split_coefficient: " + std::to_string(data[date].split_coefficient);
                out += "\n\n";
            }
            return out;
        }
    
    private:
        const string expected_return = "timestamp,open,high,low,close,adjusted_close,volume,dividend_amount,split_coefficient";
        const string url_base = "https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY_ADJUSTED";
        
        string symbol;                  ///< Stock symbol, set on initialization
        string url;                     ///< API call URL generated on initialization
        string key;                     ///< API key set in function call
        string readBuffer;              ///< Read buffer holds raw data input as string
        string outputsize = "compact";  ///< Output size to be used in API call (either compact or full)
        
        CURL * easyhandle;              ///< Pointer to CURL object (used for API calls)
        StockMap data;                  ///< Hashmap mapping dates to StockDataEntry elements
        vector<string> datev;           ///< Date vector, used for querying the stockmap and holding dates in order
        
        struct curl_slist * headers; //don't put variables below this (causes seg fault?!)
        
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
            url = url_base;
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
                cerr << k << endl;
                return;
            }
            
            for (k = strtok(NULL, "\n"); k != NULL; k = strtok(NULL, "\n")) 
                unpacked.push_back(k);
            
            for (string line : unpacked) {
                StockDataEntry temp = parseLine(line);
                data[temp.date] = temp;
                datev.push_back(temp.date);
            }
        }

};

int main(int argc, char * argv[]) {
    /* --- Test: Microsoft --- */
    StockData * Microsoft = new StockData("MSFT");
    Microsoft->setAPIKey("ff4b7dad06mshe8f6632474c0fa5p14aba0jsn5304c3e949f5");
    Microsoft->setOutputSize("full");
    Microsoft->populateData();

    StockData::StockMap MicrosoftData = Microsoft->getData();
    cout << "On 2020-12-17, Microsoft opened at ";
    cout << MicrosoftData["2020-12-17"].open << " and closed at ";
    cout << MicrosoftData["2020-12-17"].close << endl;
    cout << endl;

    vector<string> msDates = Microsoft->getDateVector();
    string symbol = Microsoft->getSymbol();
    int length = Microsoft->getLength();

    cout << "Symbol, valid: " << (msDates.size() == length) << endl;
    cout << "First date: " << Microsoft->getFirstDate() << endl;
    cout << "Last date: " << Microsoft->getLastDate() << endl;
    cout << "The two-hundreth date was " << Microsoft->getNthDate(200) << endl;
    cout << endl;

    cout << "For " << Microsoft->getSymbol() << " 50 days ago: " << endl;
    cout << "\t 10 day moving average was " << Microsoft->getNDayAverage(10, 50) << endl;
    cout << "\t 100 day moving average was " <<  Microsoft->getNDayAverage(100, 50) << endl;
    cout << "\t 200 day moving average was " <<  Microsoft->getNDayAverage(200, 50) << endl; 
    cout << endl;
    
    
    cout << "For " << Microsoft->getSymbol() << " on " << Microsoft->getLastDate() << endl;
    cout << "\t 10 day moving average was " << Microsoft->getNDayAverage(10) << endl;
    cout << "\t 100 day moving average was " <<  Microsoft->getNDayAverage(100) << endl;
    cout << "\t 200 day moving average was " <<  Microsoft->getNDayAverage(200) << endl; 
    cout << endl;
    
    return 0;
}
