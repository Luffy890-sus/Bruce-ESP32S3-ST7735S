#include "core/powerSave.h"
#include <interface.h>

// ---------------------------------------------------------------------------
// Board profile: ESP32-S3-N16R8 + ST7735S 128x160 (SPI)
//
// Power: this DevKit-style profile has no onboard battery gauge or power rail
// control, so battery reporting and power-off are stubbed out.
//
// UI: three momentary buttons wired between the GPIO and GND. Internal
// pull-ups are enabled in _setup_gpio(), so no external resistors are needed.
//   UP   (UP_BTN)  -> previous item / up
//   SEL  (SEL_BTN) -> select (OK); long press -> back / ESC
//   DOWN (DW_BTN)  -> next item / down
//
// Pin numbers can be overridden from the .ini with -DUP_BTN=.. etc.
// ---------------------------------------------------------------------------

#ifndef UP_BTN
#define UP_BTN 5
#endif

#ifndef SEL_BTN
#define SEL_BTN 4
#endif

#ifndef DW_BTN
#define DW_BTN 6
#endif

// Buttons short the GPIO to GND, so LOW means pressed.
#ifndef BTN_ACTIVE
#define BTN_ACTIVE LOW
#endif

#ifndef BTN_DEBOUNCE_MS
#define BTN_DEBOUNCE_MS 120
#endif

#ifndef BTN_LONGPRESS_MS
#define BTN_LONGPRESS_MS 700
#endif

static const bool kBtnIdle = (BTN_ACTIVE == LOW) ? HIGH : LOW;

static bool upState = kBtnIdle;
static bool selState = kBtnIdle;
static bool dwState = kBtnIdle;

static uint32_t upEdgeMs = 0;
static uint32_t selEdgeMs = 0;
static uint32_t dwEdgeMs = 0;

// Suppresses the short-press event when the same hold already fired a long press.
static bool selLongSent = false;

void _setup_gpio() {
    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);

    bruceConfig.startupApp = "WebUI";
}

int getBattery() { return 0; }

bool isCharging() { return false; }

void _setBrightness(uint8_t brightval) { (void)brightval; }

void powerOff() {}

void checkReboot() {}

void InputHandler(void) {
    const uint32_t now = millis();

    // --- UP ------------------------------------------------------------
    bool upNow = digitalRead(UP_BTN);
    if (upNow != upState && now - upEdgeMs > BTN_DEBOUNCE_MS) {
        upEdgeMs = now;
        if (upNow == BTN_ACTIVE) {
            UpPress = true;
            PrevPress = true;
            AnyKeyPress = true;
        }
    }
    upState = upNow;

    // --- DOWN ----------------------------------------------------------
    bool dwNow = digitalRead(DW_BTN);
    if (dwNow != dwState && now - dwEdgeMs > BTN_DEBOUNCE_MS) {
        dwEdgeMs = now;
        if (dwNow == BTN_ACTIVE) {
            DownPress = true;
            NextPress = true;
            AnyKeyPress = true;
        }
    }
    dwState = dwNow;

    // --- SEL: short press = OK, long press = ESC -----------------------
    bool selNow = digitalRead(SEL_BTN);
    if (selNow != selState && now - selEdgeMs > BTN_DEBOUNCE_MS) {
        selEdgeMs = now;
        if (selNow == BTN_ACTIVE) {
            selLongSent = false;
        } else if (!selLongSent) {
            SelPress = true;
            AnyKeyPress = true;
        }
    }
    selState = selNow;

    if (selNow == BTN_ACTIVE && !selLongSent && now - selEdgeMs >= BTN_LONGPRESS_MS) {
        selLongSent = true;
        EscPress = true;
        AnyKeyPress = true;
    }
}
