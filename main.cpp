// compilieren mit g++ main.c -std=c++0x -o main -pthread -lcurl -lwiringPi

// http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=südhöhe
#include <iostream>
#include <time.h>

#include <vector>
#include <curl/curl.h> // apt-get install libcurl4-gnutls-dev
#include <string>
#include <unistd.h>

#include <thread>
#include <mutex>

extern "C" {
   #include "ST7565.h"
}

#define contrast 30

typedef struct{
   std::string name;
   std::string url;
}Bus_Stop_Config_t;

#define sizeofarray(arr) ((sizeof(arr))/(sizeof(arr[0])))

static Bus_Stop_Config_t bus_stop_config[] = { 
   {"Saarstrasse"   , "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=Saarstrasse"},
   {"Cunnersd. Str.", "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=CunnersdorferStrasse"}
};

typedef struct{
   std::string s_minutes;     // z.B. 5
   std::string s_destination; // z.B. Coschütz
   std::string s_description; // z.B. 66
} Bus_Line_Information_t;

static std::mutex bus_stop_information_mutex;
static std::vector<std::vector<Bus_Line_Information_t>> bus_stop_information(sizeofarray(bus_stop_config));

static bool parse_bus_stop_information( std::string http_content, std::vector<Bus_Line_Information_t>& bus_stop_information );
static void replace_utf_letters( std::string& s );

// TODO: get Raspberry GPIO Running http://raspberrypiguide.de/howtos/raspberry-pi-gpio-how-to/
// http://www.element14.com/community/servlet/JiveServlet/previewBody/73950-102-4-309126/GPIO_Pi2.png?01AD=3GZgWMTpg1jDSMy2YO8hD1OqtjdCp9XqniBSBHoUhsn1I8J5raM6jOQ&01RI=B51CF70A37220D9&01NA=na

static int min( int a, int b ){
   if( a < b ) return a;
   return b;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
   ((std::string*)userp)->append((char*)contents, size * nmemb);
   return size * nmemb;
}

/*
*  \brief Performs HTML request and stores response in content
*
*  Blocks while fetching the content!
*
*  \param [in]  url
*  \param [in]  pointer to content, gets written to in function "WriteCallback"
*  \return CURLcode for error checking 
*/
static CURLcode get_bus_stop_information( const std::string& url, std::string* content ){
   CURL *curl;
   CURLcode res;
   // get bus/tram information
   curl = curl_easy_init();
   if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, content);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);      // Max 20 seconds
      res = curl_easy_perform(curl);
      curl_easy_cleanup(curl);
  }
  return res;
}


/*
*  \brief Cyclicly calls get_bus_stop_information for the configured bus stops and parses the data into bus_stop_information
*
*  Should never return
*/
static void get_bus_stop_information_worker( ){
   while(true){
      for( int i = 0; i < sizeofarray(bus_stop_config); i++ ){
         std::string content;
         CURLcode return_value = get_bus_stop_information(bus_stop_config[i].url, &content);
         if( return_value == CURLE_OK ){
            std::vector<Bus_Line_Information_t> bus_stop_information_;
            if( parse_bus_stop_information( content, bus_stop_information_ ) ){
               bus_stop_information_mutex.lock();
               bus_stop_information[i] = bus_stop_information_;
               bus_stop_information_mutex.unlock();
            }else{
               std::cerr << "Error while parsing: " << content << std::endl;
            }
         }else{
            std::cerr << "get_bus_stop_information(" << bus_stop_config[i].url << ") failed with code: " << return_value << std::endl;
         }
      }
      sleep(30);
   }
}

/*
*  \brief Creates the displayed text out of the bus_line_info
*
*  \param [in/out] text:          Pointer to allocated char array where the text is being stored
*  \param [in]     bus_line_info: Bus line information that should be displayed
*/
void createText(char* text, const Bus_Line_Information_t& bus_line_info){
   // TODO: Check length of strings
   sprintf( text   , "%2s %-15s", bus_line_info.s_description.c_str(), bus_line_info.s_destination.c_str() );
   sprintf( text+18, " %2s", bus_line_info.s_minutes.c_str() );
}


int main(int argc, char** argv){
   std::cout << "Busplaner gestartet" << std::endl;

   curl_global_init( CURL_GLOBAL_ALL );

   // initialize screen
   st7565_init(contrast);

   std::thread thread_information_worker( get_bus_stop_information_worker );

   int current_display_bus_stop = 0;

   // main-loop
   while(1){    
      // clear buffer and screen
      st7565_clear();

      time_t current_time;
      struct tm * time_info;
      char time_string[8];

      time(&current_time);
      time_info = localtime(&current_time);

      strftime(time_string, sizeof(time_string), " %H:%M", time_info);	

      drawstring(0, 0, bus_stop_config[current_display_bus_stop].name.c_str());
      drawstring(15*6, 0, time_string);

      bus_stop_information_mutex.lock();
      for(unsigned i = 0; i < min( 7, bus_stop_information[current_display_bus_stop].size() ); i++){
         char text[64];
         createText(text, bus_stop_information[current_display_bus_stop][i]);

         //std::cout << text << std::endl;
        drawstring(0, i+1, text);
      }
      bus_stop_information_mutex.unlock();

      // update screen
      st7565_display(); 

      sleep(5);
      current_display_bus_stop = (current_display_bus_stop+1)%sizeofarray(bus_stop_config);
      //std::cout << "--------------" << std::endl;
   }   // end while

   return 0; 
}

static void replace_utf_letters( std::string& s ){
   for( int i = 0; i < s.size(); i++ ){
      if( uint8_t(s[i]) != 0xC3 ) continue;
      switch( uint8_t(s[i+1]) ){
         case 0x84:
            s[i] = 'A'; s[i+1] = 'e'; i++; break;
         case 0x96:
         	s[i] = 'O'; s[i+1] = 'e'; i++; break;	
         case 0x9C:
         	s[i] = 'U'; s[i+1] = 'e'; i++; break;	
         case 0xA4:
         	s[i] = 'a'; s[i+1] = 'e'; i++; break;		
         case 0xB6:
         	s[i] = 'o'; s[i+1] = 'e'; i++; break;	
         case 0xBC:
         	s[i] = 'u'; s[i+1] = 'e'; i++; break;
         default: 
         	std::cout << "UTF-Charakter nicht unterstuetzt" << std::endl; 
      }        
   }
}


/* --------------------------------
    parsing functions
   --------------------------------
*/ 

// trim from left
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
   s.erase(0, s.find_first_not_of(t));
   return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
   s.erase(s.find_last_not_of(t) + 1);
   return s;
}

// trim from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
   return ltrim(rtrim(s, t), t);
}


static bool remove_quotes( std::string& input ){
   if( input.size() < 2 ) return false;
   if( input[0]              != '"' && input[0]              != '\'' ) return false;
   if( input[input.size()-1] != '"' && input[input.size()-1] != '\'' ) return false;
   input = trim( input, "'\"" );
   return true;
}

//std::string s_description; // z.B. 66
//std::string s_destination; // z.B. Coschütz
//std::string s_minutes;     // z.B. 5
static bool parse_bus_line_information( const std::string& bus_line_string, Bus_Line_Information_t* bus_line_information ){   

   std::size_t index = bus_line_string.find(',');
   if( index == std::string::npos ) return false;

   bus_line_information->s_description = bus_line_string.substr( 0, index );
   if( !remove_quotes( bus_line_information->s_description ) ) return false;
   replace_utf_letters( bus_line_information->s_description );

   std::size_t index_destination = bus_line_string.find(',', index+1);
   if( index_destination == std::string::npos ) return false;
   bus_line_information->s_destination = bus_line_string.substr( index+1, index_destination-index-1 );
   if( !remove_quotes( bus_line_information->s_destination ) ) return false;
   replace_utf_letters( bus_line_information->s_destination );

   bus_line_information->s_minutes = bus_line_string.substr( index_destination+1 );
   if( !remove_quotes( bus_line_information->s_minutes ) ) return false;
   replace_utf_letters( bus_line_information->s_minutes );

   return true;
}

static bool parse_bus_stop_information( std::string http_content, std::vector<Bus_Line_Information_t>& bus_stop_information ){
   http_content = trim( http_content, " \n\r\t");
   if( http_content[0] != '[' ){
      std::cerr << "Expected leading '['" << std::endl;
      return false;
   }
   for(int i = 1; i < http_content.size(); i++){
      if( http_content.at(i) == '['){
         Bus_Line_Information_t bus_line_information;
         
         std::size_t end = http_content.find(']',i+1);
         if( end == std::string::npos ){
            std::cerr << "Could not find closing bracket ]" << std::endl;
            return false; 
         }

         bool success = parse_bus_line_information( http_content.substr( i+1, end-i-1), &bus_line_information );
         if( success ){
            bus_stop_information.push_back( bus_line_information );
            i += (end - i);
         }else{
            std::cerr << "Failed to parse bus_line_information:" << http_content.substr( i+1, end-i-1) << std::endl; 
            return false;
         }
      }
   }
   return true;
}
