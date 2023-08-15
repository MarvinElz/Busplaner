ST7565.o: font.h ST7565.c ST7565.h ST7565-config.h
	gcc -o ST7565.o -lwiringPi

Busplaner: main.cpp ST7565.o
	g++ main.cpp ST7565.o -o main -pthread -lcurl
