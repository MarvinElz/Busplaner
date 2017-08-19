/*

 Pi wiringPi        wiringPi Pi
 17  3.3V    |o o |    5     18
 19  12      |o o |  GND     20
 21  13      |o o |    6     22
 23  14      |o o |   10     24

*/

// Setup for ST7565R in SPI mode
/** The chip select pin */
#define CS 5  // PIN 18

/** The reset pin (this is required and should not be tied high) */
#define RESET 12  // PIN 19

/** The A0 pin, which selects command or data mode */
#define A0 13  // PIN 21

/** The clock pin */
#define SCL 6  // PIN 22

/** The data pin */
#define SDA 14  // PIN 23

/** Screen width in pixels (tested with 128) */
#define SCREEN_WIDTH 128
/** Screen height in pixels (tested with 64) */
#define SCREEN_HEIGHT 64
/** Define this if your screen is incorrectly shifted by 4 pixels */
#define ST7565_REVERSE
/** By default we only write pages that have changed. Undefine this
if you want less/faster code at the expense of more SPI operations. */
#undef ST7565_DIRTY_PAGES
