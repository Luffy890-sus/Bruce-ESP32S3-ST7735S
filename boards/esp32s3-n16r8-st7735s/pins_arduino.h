#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

#define USB_VID 0x303a
#define USB_PID 0x1001

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t TXD2 = 1;
static const uint8_t RXD2 = 2;

static const uint8_t SDA = 13;
static const uint8_t SCL = 15;

static const uint8_t SS = 10;
static const uint8_t MOSI = 17;
static const uint8_t MISO = 19;
static const uint8_t SCK = 18;

static const uint8_t G0 = 0;
static const uint8_t G1 = 1;
static const uint8_t G2 = 2;
static const uint8_t G3 = 3;
static const uint8_t G4 = 4;
static const uint8_t G5 = 5;
static const uint8_t G6 = 6;
static const uint8_t G7 = 7;
static const uint8_t G8 = 8;
static const uint8_t G9 = 9;
static const uint8_t G10 = 10;
static const uint8_t G11 = 11;
static const uint8_t G12 = 12;
static const uint8_t G13 = 13;
static const uint8_t G14 = 14;
static const uint8_t G15 = 15;
static const uint8_t G39 = 39;
static const uint8_t G40 = 40;
static const uint8_t G41 = 41;
static const uint8_t G42 = 42;
static const uint8_t G43 = 43;
static const uint8_t G44 = 44;
static const uint8_t G46 = 46;

static const uint8_t ADC1 = 7;
static const uint8_t ADC2 = 8;

#define RGB_LED 48
#define PIN_RGB_LED 48
#define LED_BUILTIN 48
#define RGB_BUILTIN 48
#define BAD_TX 21
#define BAD_RX 22

// SERIAL (GPS) dedicated pins
#define SERIAL_TX 21
#define SERIAL_RX 22
#define GPS_SERIAL_TX SERIAL_TX
#define GPS_SERIAL_RX SERIAL_RX
#define TXLED -1
#define LED_ON HIGH
#define LED_OFF LOW

// CC1101 sub-GHz module hangs off the shared SPI bus declared below.
// CSN/GDO0/GDO2 use free GPIOs on this board and can be overridden from the
// .ini with -DCC1101_SS_PIN=.. / -DCC1101_GDO0_PIN=.. / -DCC1101_GDO2_PIN=..
#ifndef CC1101_GDO0_PIN
#define CC1101_GDO0_PIN 14
#endif

#ifndef CC1101_GDO2_PIN
#define CC1101_GDO2_PIN 47
#endif

#ifndef CC1101_SS_PIN
#define CC1101_SS_PIN 12
#endif

#ifndef CC1101_MOSI_PIN
#define CC1101_MOSI_PIN SPI_MOSI_PIN
#endif

#ifndef CC1101_SCK_PIN
#define CC1101_SCK_PIN SPI_SCK_PIN
#endif

#ifndef CC1101_MISO_PIN
#define CC1101_MISO_PIN SPI_MISO_PIN
#endif

#ifndef NRF24_CE_PIN
#define NRF24_CE_PIN -1
#endif

#ifndef NRF24_SS_PIN
#define NRF24_SS_PIN -1
#endif

#ifndef NRF24_MOSI_PIN
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#endif

#ifndef NRF24_SCK_PIN
#define NRF24_SCK_PIN SPI_SCK_PIN
#endif

#ifndef NRF24_MISO_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN
#endif

// FP/FM/FG font size defines are in precompiler_flags.h to avoid
// conflicts with FastLED which uses FP as a type alias.

#define HAS_SCREEN 1
#define ROTATION 1
#define MINBRIGHT 160

#define SDCARD_CS -1
#define SDCARD_SCK -1
#define SDCARD_MISO -1
#define SDCARD_MOSI -1

#define GROVE_SDA -1
#define GROVE_SCL -1

#define SPI_SCK_PIN 18
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 17
#define SPI_SS_PIN 10

#endif /* Pins_Arduino_h */
