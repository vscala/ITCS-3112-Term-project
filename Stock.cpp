#include <iostream>
#include <string>
#include <curl/curl.h>
//#include <rapidjson/document.h>

//using namespace rapidjson;
using namespace std;

class StockData {
    private:
        string symbol;
        string url;
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
            headers = curl_slist_append(headers, "x-rapidapi-key: 623d0d283fmsh0ece875b6aac448p15fa6ejsn17c7de05a48d");
            headers = curl_slist_append(headers, "x-rapidapi-host: alpha-vantage.p.rapidapi.com");
            curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, headers);
        }
        /*
        
        void todo() {
            CURLcode ret = curl_easy_perform(easyhandle);
            curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &readBuffer);

            curl_easy_perform(easyhandle);

            Document document;
            document.Parse(readBuffer.c_str());

            std::cout << document["Time Series (Daily)"].GetString() << std::endl;
            
        }*/

    public:
        StockData(string symbol) {
            this->symbol = symbol;
            this->url = "https://alpha-vantage.p.rapidapi.com/query?function=TIME_SERIES_DAILY_ADJUSTED&symbol=" + symbol + "&outputsize=full&datatype=json";
            initializeCurl();
            setHeaders();
        }      
};

int main(int argc, char * argv[]) {
    StockData Microsoft("MSFT");
    return 0;
}
