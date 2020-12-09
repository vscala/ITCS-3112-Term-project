/*! \mainpage Stock Indicator
 *
 * \section data_sec Stock Data
 *
 * \ref StockData
 *
 * \section indic_sec Stock Indicators
 *
 * \ref StockIndicators
 *
 */

class StockData
{
    private:
        FILE * file;
        Curl * curl;
        
    public:

        /// Create StockData
        StockData();

        /// Create StockData with this symbol
        StockData(string symbol);

        ~StockData();
        
        /** 
         * \return json file with stock data
         */ 
        FILE getData();

        /**
         * \return this stock symbol
         */ 
        string getSymbol();
        /**
         * \param stock symbol
         */ 
        void setSymbol(string symbol);
};  

class StockIndicators {
    private:
        queue<double> 50day;
        double 50dMA;
        queue<double> 100day;
        double 100dMA;
        queue<double> 200day;
        double 200dMA;
        
    public:
        /// Create StockIndicators class
        StockIndicators();
        
        ~StockIndicators();
    
    
        /**
         * \param new amount to push to each queue
         * \brief push new amount and pop queues if full, update moving averages
         */ 
        void push(double amount);
        
        /**
         * \return 50 day moving average
         */ 
        double get50dMA();
        /**
         * \return 100 day moving average
         */ 
        double get100dMA();
        /**
         * \return 200 day moving average
         */ 
        double get200dMA();

};  
