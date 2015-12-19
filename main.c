// compilieren mit g++ main.c -o main -lcurl -lwiringPi

// http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=südhöhe
#include <iostream>
#include <vector>
#include <curl/curl.h> // apt-get install libcurl4-gnutls-dev
#include <string>
#include <unistd.h>
#include <wiringPi.h>
#include "ST7565.c"

#define contrast 30

using namespace std;

typedef vector<string> line;

vector<line> parseHTML( string html );


vector<line> haltestelle;


// TODO: get Raspberry GPIO Running http://raspberrypiguide.de/howtos/raspberry-pi-gpio-how-to/
// http://www.element14.com/community/servlet/JiveServlet/previewBody/73950-102-4-309126/GPIO_Pi2.png?01AD=3GZgWMTpg1jDSMy2YO8hD1OqtjdCp9XqniBSBHoUhsn1I8J5raM6jOQ&01RI=B51CF70A37220D9&01NA=na

 
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char** argv){
  cout << "Hallo Welt" << endl;
  CURL *curl;
  CURLcode res;
  string body;
  wiringPiSetup();

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=südhöhe");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    haltestelle = parseHTML( body ); 
    for(unsigned i = 0; i < haltestelle.size(); i++){
       for(unsigned j = 0; j < haltestelle[i].size(); j++)
           cout << haltestelle[i][j] << " ";
       cout << endl;
    }
  }
  

  // initialize screen
  st7565_init(contrast);

  // clear buffer and screen
  st7565_clear();

  // Hello World
  char helloworld[] = "Hello World";
  drawstring(1, 1, helloworld);

  // update screen
  st7565_display();

  // main-loop
  //while(1){
    // get bus/tram information

    
    // parse information into vector<line>
    
    // prepare information 
  //}
 
}

/* --------------------------------
    parsing functions
   --------------------------------
*/ 


string parseInfo( string html ){
   return html.substr(1, html.size()-2);
}

line parseLine( string html ){   
   line help;
   for(int i = 0; i < html.size(); i++){
      if( html.at(i) == ',') 
      {
         i++;         
         continue;
      }
      help.push_back( parseInfo( html.substr (i, html.find(',',i)-i ) ) );           
      i = html.find(',',i);
      if( i < 0 ) break;
   }
   return help;
}

vector<line> parseHTML( string html ){
   vector<line> halt;
   for(int i = 0; i < html.size(); i++){
      if( html.at(i) == '[' || html.at(i) == ',' || html.at(i) == ']') 
      {
         i++;
         continue;
      }
      halt.push_back( parseLine(html.substr (i, html.find(']',i)-i ) ) );      
      i = html.find(']',i);
      if( i < 0 ) break;
   }
   return halt;
}
