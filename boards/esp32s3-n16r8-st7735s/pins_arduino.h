#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

// ---------------------------------------------------------------------------
// ESP32-S3-N16R8 board with ST7735S 128x160 + CC1101 + 3 buttons.
//
// Pin map taken from the vendor pinout of this board:
//   GPIO1  PN532 SCL          GPIO13 Encoder B
//   GPIO2  PN532 SDA          GPIO14 Encoder A
//   GPIO3  Display BLK        GPIO15 CC1101 CSN
//   GPIO4  IR RX              GPIO16 CC1101 GDO0
//   GPIO5  IR TX              GPIO17 CC1101 MOSI
//   GPIO6  CC1101 GDO2        GPIO18 CC1101 SCK
//   GPIO7  nRF24 CE           GPIO19 SD MISO
//   GPIO8  CC1101 MISO        GPIO20 SD SCK
//   GPIO9  Display DC         GPIO21 free (LEFT button)
//   GPIO10 Display RES        GPIO39 GPS RX<-TX   GPIO40 GPS TX->RX
//   GPIO11 Display SDA/MOSI   GPIO41 SD MOSI      GPIO42 SD CS
//   GPIO12 Display SCL/SCK    GPIO46 Display CS   GPIO47 free (RIGHT button)
//   GPIO48 Encoder SW / SELECT
//
// GPIO26-32 are used by SPI flash and GPIO33-37 by the octal PSRAM of the
// N16R8 module, so the vendor's nRF24 assignment (35/36/37/38) is NOT usable
// on this module. Everything used here avoids those pins.
// ---------------------------------------------------------------------------

#define USB_VID 0x303a
#define USB_PID 0x1001

// UART0 -> CH343 USB bridge (flashing + logs)
static const uint8_t TX = 43;
static const uint8_t RX = 44;

// UART1 on the GPS header
static const uint8_t TXD2 = 40;
static const uint8_t RXD2 = 39;

// I2C: PN532 header per vendor pinout. Never use GPIO10/GPIO15 here, they are
// the display reset and the CC1101 chip select.
static const uint8_t SDA = 2;
static const uint8_t SCL = 1;

// SD-card SPI header per vendor pinout
static const uint8_t SS = 42;
static const uint8_t MOSI = 41;
static const uint8_t MISO = 19;
static const uint8_t SCK = 20;

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
static const uint8_t G16 = 16;
static const uint8_t G17 = 17;
static const uint8_t G18 = 18;
static const uint8_t G19 = 19;
static const uint8_t G20 = 20;
static const uint8_t G21 = 21;
static const uint8_t G39 = 39;
static const uint8_t G40 = 40;
static const uint8_t G41 = 41;
static const uint8_t G42 = 42;
static const uint8_t G43 = 43;
static const uint8_t G44 = 44;
static const uint8_t G46 = 46;
static const uint8_t G47 = 47;
static const uint8_t G48 = 48;

static const uint8_t ADC1 = 7;
static const uint8_t ADC2 = 8;

// No addressable LED on this board; GPIO48 is the SELECT button.
#define RGB_LED -1
#define PIN_RGB_LED -1
#define LED_BUILTIN -1
#define RGB_BUILTIN -1

// BadUSB is compiled out (ARDUINO_USB_MODE=0), so claim no UART pins for it.
#define BAD_TX -1
#define BAD_RX -1

// GPS / RFID125 UART: GPIO40 = ESP TX, GPIO39 = ESP RX
#define SERIAL_TX 40
#define SERIAL_RX 39
#define GPS_SERIAL_TX SERIAL_TX
#define GPS_SERIAL_RX SERIAL_RX
#define TXLED -1
#define LED_ON HIGH
#define LED_OFF LOW

// CC1101 sits on its own SPI signals, not on the display bus.
// Overridable from the .ini with -DCC1101_...=
#ifndef CC1101_GDO0_PIN
#define CC1101_GDO0_PIN 16
#endif

#ifndef CC1101_GDO2_PIN
#define CC1101_GDO2_PIN 6
#endif

#ifndef CC1101_SS_PIN
#define CC1101_SS_PIN 15
#endif

#ifndef CC1101_MOSI_PIN
#define CC1101_MOSI_PIN 17
#endif

#ifndef CC1101_SCK_PIN
#define CC1101_SCK_PIN 18
#endif

#ifndef CC1101_MISO_PIN
#define CC1101_MISO_PIN 8
#endif

// nRF24 is not usable on this module: the vendor map puts it on GPIO35-37,
// which the octal PSRAM of the N16R8 owns.
#ifndef NRF24_CE_PIN
#define NRF24_CE_PIN -1
#endif

#ifndef NRF24_SS_PIN
#define NRF24_SS_PIN -1
#endif

#ifndef NRF24_MOSI_PIN
#define NRF24_MOSI_PIN 17
#endif

#ifndef NRF24_SCK_PIN
#define NRF24_SCK_PIN 18
#endif

#ifndef NRF24_MISO_PIN
#define NRF24_MISO_PIN 8
#endif

// FP/FM/FG font size defines are in precompiler_flags.h to avoid
// conflicts with FastLED which uses FP as a type alias.

#define HAS_SCREEN 1
#define ROTATION 1
#define MINBRIGHT 160

// The display and CC1101 use separate SPI signal pairs, so there is no shared
// bus here. SD pins describe the SD header, disabled by default.
#define SDCARD_CS -1
#define SDCARD_SCK 20
#define SDCARD_MISO 19
#define SDCARD_MOSI 41

// Single-wire IR / RF helper pins per vendor pinout (IR TX / IR RX).
#define GROVE_SDA 5
#define GROVE_SCL 4

// Bruce uses SPI_* as the default bus for CC1101/NRF24/W5500.
#define SPI_SCK_PIN 18
#define SPI_MISO_PIN 8
#define SPI_MOSI_PIN 17
#define SPI_SS_PIN 15

#endif /* Pins_Arduino_h */
