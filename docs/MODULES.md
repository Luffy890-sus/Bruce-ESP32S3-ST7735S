# Внешние модули

Порт по умолчанию включает только дисплей и кнопки — все радио-модули
выключены (`-1` в пинах). Ниже — что можно подключить и какие флаги раскомментировать.

## Итоговая карта GPIO

| GPIO | Занят | Комментарий |
|---|---|---|
| 0 | BOOT | strapping, не трогать |
| 1 | *(свободно)* | W5500 INT / SD CS |
| 2 | *(свободно)* | W5500 SS |
| 3 | strapping | JTAG select |
| **4** | **кнопка SEL** | |
| **5** | **кнопка UP** | |
| **6** | **кнопка DOWN** | |
| 7 | NRF24 CE | по умолчанию выключен |
| **8** | **TFT CS** | |
| **9** | **TFT DC** | |
| **10** | **SPI SS (общий)** | |
| 11 | *(свободно)* | |
| **12** | **CC1101 CSN** | |
| 13 | I2C SDA | |
| **14** | **CC1101 GDO0** | |
| **15** | **TFT RST** | он же `SCL` в `pins_arduino.h` — см. ниже |
| **17** | **SPI MOSI / TFT MOSI** | |
| **18** | **SPI SCK / TFT SCK** | |
| **19** | **SPI MISO** | он же native USB `D-` — конфликт при `ARDUINO_USB_MODE=1` |
| 20 | native USB `D+` | |
| **21** | **TFT BL** | он же `BAD_TX`, `SERIAL_TX` (UART1) |
| 22 | `BAD_RX`, `SERIAL_RX` (UART1) | UART для GPS / RFID125 |
| 26–32 | flash | **занято, недоступно** |
| 33–37 | octal PSRAM | **занято (модификация N16R8)** |
| 38 | IR TX (`GROVE_SDA`) | |
| 39 | IR RX (`GROVE_SCL`) | |
| 40 | TXLED | |
| 41 | I2S DATA | микрофон |
| 42 | I2S BCLK | микрофон |
| 43 | UART0 TX | логирование |
| 44 | UART0 RX | логирование |
| 45, 46 | strapping | VDD_SPI / ROM select |
| 47 | **CC1101 GDO2** | |
| 48 | RGB LED | |

---

## Sub-GHz: CC1101 (433 / 868 / 915 МГц)

**Уже включён в этом порту** — пины зашиты в `pins_arduino.h`. Нужно только
выбрать модуль на устройстве:

```
Меню Bruce → Sub-Ghz → RF Module → CC1101
```

| CC1101 | GPIO |
|---|---|
| MOSI | GPIO17 (общая SPI) |
| MISO | GPIO19 (общая SPI) |
| SCK | GPIO18 (общая SPI) |
| CSN/SS | **GPIO12** |
| GDO0 | **GPIO14** |
| GDO2 | **GPIO47** (опция) |
| VCC | 3.3 V + 10 мкФ и 100 нФ рядом с модулем |
| GND | GND |

Переопределить пины можно из `.ini` (значения под `#ifndef`):

```ini
    -DCC1101_SS_PIN=12
    -DCC1101_GDO0_PIN=14
    -DCC1101_GDO2_PIN=47
```

или прямо на устройстве — `Settings → CC1101 Pins` (сохранится в NVS).

Схема подключения, развязка по питанию и длина антенны —
[SETUP_GUIDE.md](SETUP_GUIDE.md#5-cc1101-sub-ghz-433-мгц).

---

## 2.4 ГГц: NRF24L01+

```ini
    -DUSE_NRF24_VIA_SPI
    -DNRF24_CE_PIN=7
    -DNRF24_SS_PIN=11
    -DNRF24_MOSI_PIN=SPI_MOSI_PIN
    -DNRF24_SCK_PIN=SPI_SCK_PIN
    -DNRF24_MISO_PIN=SPI_MISO_PIN
```

| NRF24L01+ | GPIO |
|---|---|
| MOSI | GPIO17 |
| MISO | GPIO19 |
| SCK | GPIO18 |
| CE | GPIO7 |
| CSN | GPIO11 |
| VCC | 3.3 V + конденсатор 10–100 мкФ |

> NRF24 очень чувствителен к просадкам питания — питайте через отдельный
> стабилизатор или хотя бы с конденсатором прямо у модуля.

**CC1101 и NRF24 могут висеть на одной SPI одновременно** — CS разные.
Но в меню Bruce активен только один радио-модуль за раз.

---

## Ethernet: W5500

```ini
    -DUSE_W5500_VIA_SPI
    -DW5500_SS_PIN=2
    -DW5500_MOSI_PIN=SPI_MOSI_PIN
    -DW5500_SCK_PIN=SPI_SCK_PIN
    -DW5500_MISO_PIN=SPI_MISO_PIN
    -DW5500_INT_PIN=1
```

---

## IR приёмник / передатчик

В этом профиле `GROVE_SDA`/`GROVE_SCL` равны `-1`. Чтобы задействовать:

```ini
    -DGROVE_SDA=38          ; IR TX / RF TX
    -DGROVE_SCL=39          ; IR RX / RF RX
    -DIR_TX_PINS='{{"IR TX", GROVE_SDA}}'
    -DIR_RX_PINS='{{"IR RX", GROVE_SCL}}'
    -DRF_TX_PINS='{{"RF TX", GROVE_SDA}}'
    -DRF_RX_PINS='{{"RF RX", GROVE_SCL}}'
    -DTXLED=40
    -DLED_ON=HIGH
    -DLED_OFF=LOW
```

Передатчик — обычный IR-светодиод через 100–330 Ω. Приёмник — TSOP38238 и т. п.

Однопроводные модули «M5 RF433T/433R» подключаются сюда же.

---

## RFID 125 кГц (RT-125A и клоны)

Работает через UART-порт Bruce (`bruceConfigPins.uart_bus`), в этом профиле
это **GPIO21 (TX) / GPIO22 (RX)**:

| RFID125 | GPIO |
|---|---|
| RX модуля | GPIO21 |
| TX модуля | GPIO22 |
| VCC | 5 V или 3.3 V (по модулю) |
| GND | GND |

Скорость жёстко 9600 8N1.

> **Внимание:** GPIO21 — он же подсветка дисплея. Если одновременно
> используются подсветка и UART1, назначьте `SERIAL_TX`/`SERIAL_RX` на
> свободные GPIO (например, 1 и 2) через `pins_arduino.h`.

---

## NFC / RFID 13.56 МГц

| Модуль | Шина | Пинов |
|---|---|---|
| PN532 (I2C) | I2C | SDA=GPIO13, SCL — см. ниже |
| PN532 (UART) | UART | GPIO21/GPIO22 + IRQ |
| MFRC522 | SPI | CS на свободный GPIO, шина 17/18/19 |
| ST25R3916 | SPI | CS на свободный GPIO |

### I2C-устройства

```ini
    -DSYS_I2C_SDA=13
    -DSYS_I2C_SCL=12
```

> **Не используйте GPIO15 для SCL**, хотя `pins_arduino.h` определяет
> `#define SCL 15`: этот пин — сброс дисплея. Любой I2C-обход
> (`Settings → I2C Finder`) дёрнет Reset экрана и погасит изображение.
> Для I2C берите GPIO12 / GPIO14 / GPIO7.

---

## Микрофон I2S (SPM1423 / INMP441)

```ini
    -DMIC_SPM1423=1
    -DPIN_CLK=42
    -DI2S_BCLK_PIN=42
    -DI2S_DATA_PIN=41
    -DPIN_DATA=41
```

---

## SD-карта по SPI

По умолчанию выключена (`SDCARD_CS = -1`). Чтобы включить:

```ini
    -DSDCARD_CS=1
    -DSDCARD_SCK=18
    -DSDCARD_MISO=19
    -DSDCARD_MOSI=17
```

Занятый GPIO1 при этом отнимается у W5500 INT.

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
