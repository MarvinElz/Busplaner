// compilieren mit g++ main.c -std=c++0x -o main -pthread -lcurl -lwiringPi

// http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=südhöhe
#include <iostream>
#include <time.h>

#include <vector>
#include <curl/curl.h> // apt-get install libcurl4-gnutls-dev
#include <string>
#include <unistd.h>
#include <wiringPi.h>

#include <thread>

#include "ST7565.c"
<<<<<<< HEAD

#define contrast 30
using namespace std;

typedef vector<string> line;

void parseHTML(  );
void remove_UTF_characters(char* text){
	
}

struct hst{
   char name[128];
   const char url[512];
}typedef hst;

#define sizeofarray(arr) ((sizeof(arr))/(sizeof(arr[0])))

//hst haltestellen[] = { {"Suedhoehe:", "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=suedhoehe" }
//                   , {"Hoeckendorfer Weg:", "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=Hoeckendorfer+Weg"} };
hst haltestellen[] = { {"Saarstrasse"       , "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=Saarstrasse"},
					   {"Cunnersd. Str.", "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=CunnersdorferStrasse"}
					 };
=======

#define contrast 30
using namespace std;

typedef vector<string> line;

void parseHTML(  );

struct hst{
   char name[100];
   const char url[300];
}typedef hst;

hst haltestellen[] = { {"Suedhoehe:", "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=suedhoehe" }
                   , {"Hoeckendorfer Weg:", "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=Hoeckendorfer+Weg"} };
>>>>>>> 92bf85c65b752df51329417bef6346ae830678c2

vector<line> haltestelle;

char text[128];
char hstIndex = 0;
string body;

void createText(char* text, const line);


// TODO: get Raspberry GPIO Running http://raspberrypiguide.de/howtos/raspberry-pi-gpio-how-to/
// http://www.element14.com/community/servlet/JiveServlet/previewBody/73950-102-4-309126/GPIO_Pi2.png?01AD=3GZgWMTpg1jDSMy2YO8hD1OqtjdCp9XqniBSBHoUhsn1I8J5raM6jOQ&01RI=B51CF70A37220D9&01NA=na

 
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int min( int a, int b ){
   if( a < b ) return a;
   return b;
}

void getInformation( ){
  CURL *curl;
  CURLcode res;
  // get bus/tram information
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, haltestellen[hstIndex].url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
}



int main(int argc, char** argv){
<<<<<<< HEAD
  cout << "Busplaner gestartet" << endl;
  
  wiringPiSetup();

  // initialize screen
  st7565_init(contrast);

  time_t start;

  // main-loop
  while(1){ 

    body.clear();

    start = time(NULL);
    thread threadgetInfo( getInformation );
    while( time(NULL) - start < 6 && !threadgetInfo.joinable() ){
      sleep(1);	
    }
    if( threadgetInfo.joinable() ) 
       threadgetInfo.join();
    else
       continue;

    haltestelle.clear();
    start = time(NULL);      
    thread threadparse( parseHTML );
    while( time(NULL) - start < 6 && !threadparse.joinable() ){
      sleep(1);	
    }
    if( threadparse.joinable() )
       threadparse.join();
    else 
       continue;

    // clear buffer and screen
    st7565_clear();


	time_t current_time;
	struct tm * time_info;
	char timeChar[7];
	timeChar[0] = ' ';
	
	time(&current_time);
	time_info = localtime(&current_time);
	
	strftime(timeChar+1, sizeof(timeChar), "%H:%M", time_info);	
	string  timeString(timeChar);
	
	string headerLine;
	
	//addTimeToHaltestellenName( timeString, headerLine,  )
	
    drawstring(0, 0, haltestellen[hstIndex].name);
    drawstring(15*6, 0, timeChar);

=======
  cout << "Hallo Welt" << endl;
  

  wiringPiSetup();

  // initialize screen
  st7565_init(contrast);

  time_t start;

  // main-loop
  while(1){
  

    body.clear();

    start = time(NULL);
    thread threadgetInfo ( getInformation );
    while( time(NULL) - start < 6 && !threadgetInfo.joinable() ){
      sleep(1);	
    }
    if( threadgetInfo.joinable() ) 
       threadgetInfo.join();
    else
       continue;


    haltestelle.clear();
    start = time(NULL);      
    thread threadparse ( parseHTML );
    while( time(NULL) - start < 6 && !threadparse.joinable() ){
      sleep(1);	
    }
    if( threadparse.joinable() )
       threadparse.join();
    else 
       continue;

    // clear buffer and screen
    st7565_clear();

    drawstring(0, 0, haltestellen[hstIndex].name);

>>>>>>> 92bf85c65b752df51329417bef6346ae830678c2
    // parse information into vector<line>
    for(unsigned i = 0; i < min( 7, haltestelle.size() ); i++){
      createText(text, haltestelle[i]);
      cout << text << endl;
      drawstring(0, i+1, text);
<<<<<<< HEAD
    }   
=======
    }
   
>>>>>>> 92bf85c65b752df51329417bef6346ae830678c2

    // update screen
    st7565_display(); 
    
<<<<<<< HEAD
    sleep(10);
    hstIndex = (hstIndex+1)%sizeofarray(haltestellen);
=======
    sleep(4);
    hstIndex = (hstIndex+1)%2;
>>>>>>> 92bf85c65b752df51329417bef6346ae830678c2
    cout << "--------------" << endl;
  }   // end while
 
}

<<<<<<< HEAD
//void addTimeToHaltestellenName(const &timeString, string &result, const string &HST_Name ){
//	//max 21 Zeichen
//	result = HST_Name;
//	result.append("                     ");
//	result.replace(15,  timeString.length() ,timeString);
//}
=======
>>>>>>> 92bf85c65b752df51329417bef6346ae830678c2

// creates Output Text out of line-information
void createText(char* text, const line lineInfo)
{
  char *c = text;
  unsigned s = 0;
  for( unsigned i = 0; i < min(2, lineInfo.size() ); i++ ){
<<<<<<< HEAD
     for( unsigned s = 0; s < lineInfo[i].size(); s++ ){ 
			if( lineInfo[i].at(s) == 0xC3 ){  // UTF Umlaut
				switch (lineInfo[i].at(s+1)){
				case 0x84:
					*c = 'A'; c++; *c = 'e'; s++; break;
				case 0x96:
					*c = 'O'; c++; *c = 'e'; s++; break;	
				case 0x9C:
					*c = 'U'; c++; *c = 'e'; s++; break;	
				case 0xA4:
					*c = 'a'; c++; *c = 'e'; s++; break;		
				case 0xB6:
					*c = 'o'; c++; *c = 'e'; s++; break;	
				case 0xBC:
					*c = 'u'; c++; *c = 'e'; s++; break;
				default: 
					cout << "UTF-Charakter nicht unterstuetzt" << endl; 
				}        
			}else{
				*c = lineInfo[i].at(s); // copy string				
			}
			c++;
=======
     for( unsigned s = 0; s < lineInfo[i].size(); s++ ){         
        *c = lineInfo[i].at(s); // copy string
	c++;
>>>>>>> 92bf85c65b752df51329417bef6346ae830678c2
     }
     *c++ = ' ';
  }

  while( c < text + 19 )
     *c++ = ' ';
  c = text + 18;
  *c++ = ' ';  

  if( lineInfo.size() == 3 ){
     for( unsigned s = 0; s < lineInfo[2].size(); s++ ){ 
        *c = lineInfo[2].at(s); // copy string
        c++;
     }
  }

  *c++ = '\0';
  *c++ = '\0';
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

void parseHTML(  ){
   for(int i = 0; i < body.size(); i++){
      if( body.at(i) == '[' || body.at(i) == ',' || body.at(i) == ']') 
      {
         i++;
         continue;
      }
      haltestelle.push_back( parseLine(body.substr (i, body.find(']',i)-i ) ) );      
      i = body.find(']',i);
      if( i < 0 ) break;
   }
}
