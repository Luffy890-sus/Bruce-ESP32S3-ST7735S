# Внешние модули

Порт по умолчанию включает только дисплей и кнопки — все радио-модули
выключены (`-1` в пинах). Ниже — что можно подключить и какие флаги раскомментировать.

## Итоговая карта GPIO

| GPIO | Подпись на плате | Занят | Комментарий |
|---|---|---|---|
| 0 | BOOT | strapping | не трогать |
| 1 | I2C SCL | I²C | PN532, BAT ADC |
| 2 | I2C SDA | I²C | PN532, BAT ADC |
| 3 | Display BLK | **подсветка TFT** | он же strapping (JTAG select) |
| 4 | IR RX | IR приёмник | он же `GROVE_SCL` |
| 5 | IR TX | IR передатчик | он же `GROVE_SDA` |
| **6** | CC1101 GDO2 | **CC1101** | |
| 7 | *(не выведен)* | — | |
| **8** | CC1101 MISO | **CC1101 SPI** | |
| **9** | Display DC | **TFT DC** | |
| **10** | Display RES | **TFT RST** | |
| **11** | Display SDA | **TFT MOSI** | |
| **12** | Display SCL | **TFT SCK** | |
| 13 | Encoder B | энкодер | в профиле не используется |
| 14 | Encoder A | энкодер | в профиле не используется |
| **15** | CC1101 CSN | **CC1101** | |
| **16** | CC1101 GDO0 | **CC1101** | |
| **17** | CC1101 MOSI | **CC1101 SPI** | |
| **18** | CC1101 SCK | **CC1101 SPI** | |
| 19 | SD MISO | SD-SPI | он же native USB `D-` |
| 20 | SD SCK | SD-SPI | он же native USB `D+` |
| **21** | LEFT | **кнопка UP** | |
| 22 | *(не выведен)* | — | |
| 26–32 | — | SPI flash | **занято, недоступно** |
| 33–37 | — | octal PSRAM | **занято (N16R8)** |
| 38 | *(не выведен)* | — | был бы IR TX |
| 39 | ESP RX (GPS) | UART2 RX | |
| 40 | ESP TX (GPS) | UART2 TX | он же `SERIAL_TX` в профиле |
| 41 | SD MOSI | SD-SPI | |
| 42 | SD CS | SD-SPI | он же `SS` в `pins_arduino.h` |
| 43 | TXD0 | UART0 TX | логи в CH343 |
| 44 | RXD0 | UART0 RX | логи в CH343 |
| 45 | — | strapping | VDD_SPI, не трогать |
| **46** | Display CS | **TFT CS** | он же strapping (ROM select) |
| **47** | RIGHT | **кнопка DOWN** | |
| **48** | SELECT | **кнопка SEL** | |

> `RGB_LED = -1`: отдельного адресуемого светодиода на плате нет, нога 48 — кнопка.

---

## Sub-GHz: CC1101 (433 / 868 / 915 МГц)

**Уже включён в этом порту** — своя отдельная SPI-шина (не дисплейная).

| CC1101 | Подпись на плате | GPIO |
|---|---|---|
| MISO | CC1101 MISO | **GPIO8** |
| MOSI | CC1101 MOSI | **GPIO17** |
| SCK | CC1101 SCK | **GPIO18** |
| CSN | CC1101 CSN | **GPIO15** |
| GDO0 | CC1101 GDO0 | **GPIO16** |
| GDO2 | CC1101 GDO2 | **GPIO6** |
| VCC | — | 3.3 V + 10 мкФ и 100 нФ рядом с модулем |
| GND | — | GND |

Переопределить пины можно из `.ini` (значения под `#ifndef`):

```ini
    -DCC1101_SS_PIN=15
    -DCC1101_GDO0_PIN=16
    -DCC1101_GDO2_PIN=6
```

или прямо на устройстве — `Settings → CC1101 Pins` (сохранится в NVS).

Схема подключения, развязка по питанию и длина антенны —
[SETUP_GUIDE.md](SETUP_GUIDE.md#5-cc1101-sub-ghz-433-мгц).

---

## 2.4 ГГц: NRF24L01+

В профиле **выключен** (`NRF24_CE_PIN = NRF24_SS_PIN = -1`). Причина: у таких плат
ноги nRF24 разведены на **GPIO35–38**, а GPIO33–37 в модификации N16**R8** заняты
octal PSRAM — работа по этой шине рвёт доступ к памяти.

Если модуль всё же нужен, назначьте CE/CSN на реально свободные ноги (например,
энкодерные GPIO13/14) и переиспользуйте шину CC1101:

```ini
    -DUSE_NRF24_VIA_SPI
    -DNRF24_CE_PIN=13
    -DNRF24_SS_PIN=14
    -DNRF24_MOSI_PIN=17
    -DNRF24_SCK_PIN=18
    -DNRF24_MISO_PIN=8
```

| NRF24L01+ | GPIO |
|---|---|
| MOSI | GPIO17 |
| MISO | GPIO8 |
| SCK | GPIO18 |
| CE | GPIO13 (пример) |
| CSN | GPIO14 (пример) |
| VCC | 3.3 V + конденсатор 10–100 мкФ |

> NRF24 очень чувствителен к просадкам питания — питайте через отдельный
> стабилизатор или хотя бы с конденсатором прямо у модуля.

**CC1101 и NRF24 на одной SPI висеть одновременно не должны** — CS разные, но
GDO/IRQ ноги начнут конфликтовать. В меню Bruce активен только один радио-модуль.

---

## Ethernet: W5500

Свободных ног под CS/INT практически нет (GPIO1/2 заняты I²C). Если модуль нужен —
освободите I²C или откажитесь от кнопок DOWN/UP:

```ini
    -DUSE_W5500_VIA_SPI
    -DW5500_SS_PIN=13
    -DW5500_INT_PIN=14
    -DW5500_MOSI_PIN=17
    -DW5500_SCK_PIN=18
    -DW5500_MISO_PIN=8
```

---

## IR приёмник / передатчик

На плате разведено: **IR TX = GPIO5**, **IR RX = GPIO4**. В профиле они назначены
как `GROVE_SDA`/`GROVE_SCL`, но сами списки `IR_TX_PINS`/`IR_RX_PINS` включать нужно
флагами:

```ini
    -DGROVE_SDA=5           ; IR TX / RF TX
    -DGROVE_SCL=4           ; IR RX / RF RX
    -DIR_TX_PINS='{{"IR TX", GROVE_SDA}}'
    -DIR_RX_PINS='{{"IR RX", GROVE_SCL}}'
    -DRF_TX_PINS='{{"RF TX", GROVE_SDA}}'
    -DRF_RX_PINS='{{"RF RX", GROVE_SCL}}'
```

Передатчик — обычный IR-светодиод через 100–330 Ω. Приёмник — TSOP38238 и т. п.

Однопроводные модули «M5 RF433T/433R» подключаются сюда же.

> Отдельного `TXLED` на плате нет (`TXLED = -1`), подсветка экрана для этой роли
> не подходит — GPIO3 strapping.

---

## RFID 125 кГц (RT-125A и клоны)

Работает через UART-порт Bruce (`bruceConfigPins.uart_bus`). В профиле
`SERIAL_TX/SERIAL_RX` = **GPIO40 / GPIO39** — это тот же разъём, что и GPS
(подписи `ESP TX` / `ESP RX`):

| RFID125 | Подпись на плате | GPIO |
|---|---|---|
| RX модуля | ESP TX (выход ESP32) | GPIO40 |
| TX модуля | ESP RX (вход ESP32) | GPIO39 |
| VCC | — | 5 V или 3.3 V (по модулю) |
| GND | — | GND |

Скорость жёстко 9600 8N1.

> GPS и RFID125 делят один порт — одновременно работает только что-то одно.

---

## NFC / RFID 13.56 МГц

| Модуль | Шина | Пинов |
|---|---|---|
| PN532 (I2C) | I2C | SDA=GPIO2, SCL=GPIO1 (разведено на плате) |
| MFRC522 | SPI | CS на свободный GPIO, шина 8/17/18 |
| ST25R3916 | SPI | CS на свободный GPIO, шина 8/17/18 |

### I2C-устройства

```ini
    -DSYS_I2C_SDA=2
    -DSYS_I2C_SCL=1
```

> **Не используйте GPIO12 для SCL и GPIO15 для SDA** при экспериментах: в этом
> профиле GPIO12 — тактовая шина дисплея, а GPIO15 — CC1101 CSN. Любой I2C-обход
> (`Settings → I2C Finder`) дёрнет эти линии и погасит изображение / собьёт
> радиомодуль. Для I2C берите GPIO1 (SCL) / GPIO2 (SDA) — они под это и разведены.

---

## Микрофон I2S (SPM1423 / INMP441)

На плате микрофон **не разведён**. Если хотите повесить внешний, свободных ног
практически нет — ближе всего энкодерные GPIO13/14:

```ini
    -DMIC_SPM1423=1
    -DPIN_CLK=13
    -DI2S_BCLK_PIN=13
    -DI2S_DATA_PIN=14
    -DPIN_DATA=14
```

---

## SD-карта по SPI

Разведена на плате (подписи `SD MISO / SCK / MOSI / CS`), но в профиле **выключена**
(`SDCARD_CS = -1`), чтобы не конфликтовать с меню. Чтобы включить:

```ini
    -DSDCARD_CS=42
    -DSDCARD_SCK=20
    -DSDCARD_MISO=19
    -DSDCARD_MOSI=41
```

> GPIO19/GPIO20 — одновременно ножки native USB. При `ARDUINO_USB_MODE=0`
> (как в этом профиле) они свободны и безопасно отдаются SD-шине.

---

## BadUSB (native USB HID)

В текущем профиле **выключен**: плата собирается с `ARDUINO_USB_MODE=0`,
чтобы `Serial` был виден на COM-порту CH343.

Чтобы включить BadUSB, отредактируйте
`boards/_boards_json/esp32s3-n16r8-st7735s.json`:

```json
"-DARDUINO_USB_MODE=1",
"-DARDUINO_USB_CDC_ON_BOOT"
```

Последствия:

* `Serial` уходит в native USB — логи на COM-порту CH343 **пропадут**;
* появляется второй COM-порт (`VID_303A:PID_1001`) — через него и смотреть логи;
* GPIO19 начинает конфликтовать с `SPI_MISO_PIN` — для CC1101/NRF24 MISO
  придётся переназначить;
* **для повторной прошивки снова нужен ручной download mode.**
