ST7565.o: font.h ST7565.c ST7565.h ST7565-config.h
	gcc -o ST7565.o -lwiringPi

Busplaner: main.cpp ST7565.o
	g++ main.cpp ST7565.o -o main -pthread -lcurl

Busplaner_Red: main.cpp
	g++ main.cpp -o main -pthread -lcurl -I C:\Users\Marvi\Bibliotheken\curl-8.2.1_5-win64-mingw\include -L C:\Users\Marvi\Bibliotheken\curl-8.2.1_5-win64-mingw\lib
