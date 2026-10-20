# Bruce Firmware — порт под ESP32-S3-N16R8 + ST7735S 128×160

Готовый профиль платы для [Bruce Firmware](https://github.com/BruceDevices/Bruce) под безымянный
китайский модуль **ESP32-S3-N16R8** (16 MB flash, 8 MB octal PSRAM) с SPI-дисплеем
**ST7735S 128×160** и управлением **тремя кнопками**.

Репозиторий содержит **только порт** (конфигурация платы + драйвер кнопок + документация),
а не форк всей прошивки. Файлы копируются в рабочее дерево Bruce Firmware и собираются
штатным `pio run`.

---

## Содержание

| Документ | О чём |
|---|---|
| **[docs/SETUP_GUIDE.md](docs/SETUP_GUIDE.md)** | **Полная инструкция подключения: питание + зарядка, экран, кнопки, CC1101** |
| [docs/INSTALL.md](docs/INSTALL.md) | Установка окружения, сборка, прошивка |
| [docs/WIRING.md](docs/WIRING.md) | Отдельные узлы подключения, питание от LiPo |
| [docs/BUTTONS.md](docs/BUTTONS.md) | Управление 3 кнопками, карта нажатий |
| [docs/MODULES.md](docs/MODULES.md) | Внешние модули: Sub-GHz, NRF24, NFC, IR, GPS и др. |
| [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) | Типовые ошибки сборки и прошивки |

---

## Готовая конфигурация

Пиновые номера соответствуют **шелкографии платы** (подписи `Display CS`,
`CC1101 MISO`, `LEFT/SELECT/RIGHT` и т. п.).

| Узел | Пины | Статус |
|---|---|---|
| Дисплей ST7735S 128×160 | CS=46, DC=9, RES=10, MOSI=11, SCLK=12, BLK=3 | в прошивке |
| 3 кнопки | UP=21 (`LEFT`), SEL=48 (`SELECT`), DOWN=47 (`RIGHT`) | в прошивке |
| CC1101 Sub-GHz | CSN=15, GDO0=16, GDO2=6 + своя SPI (MISO=8, MOSI=17, SCK=18) | в прошивке |
| SD-карта (SPI) | CS=42, MOSI=41, MISO=19, SCK=20 | разведена на плате, в профиле выключена |
| I²C (PN532, BAT ADC) | SCL=1, SDA=2 | в профиле выключен |
| IR | TX=5, RX=4 | в профиле выключен |
| GPS (UART2) | TX=40, RX=39 | в профиле выключен |
| Питание | LiPo 3.7 V (charge на плате) либо USB-C 5 V | см. инструкцию |
| Логи | UART0 (TX=43, RX=44) → CH343 → COM-порт, 115200 | работает |
| BadUSB (native USB HID) | — | выключен (`ARDUINO_USB_MODE=0`) |

CC1101 нужно один раз выбрать на устройстве: **Sub-Ghz → RF Module → CC1101**.

### Готовая прошивка

В репозитории лежит собранный образ под эту конфигурацию —
[`firmware/Bruce-esp32s3-n16r8-st7735s.factory.bin`](firmware/Bruce-esp32s3-n16r8-st7735s.factory.bin)
(4.1 MB). Перевести плату в download mode (BOOT + RESET) и прошить в нулевой адрес:

```bash
esptool --chip esp32s3 --port COM3 --baud 460800 write_flash 0x0 \
  firmware/Bruce-esp32s3-n16r8-st7735s.factory.bin
```

---

## Железо

| Параметр | Значение |
|---|---|
| MCU | ESP32-S3 QFN56, rev v0.2, 240 МГц |
| Flash | 16 MB (QIO) |
| PSRAM | 8 MB octal (AP_3v3), `qio_opi` |
| Дисплей | ST7735S, 128×160, TFT, redtab, `TFT_BGR` |
| Поворот экрана | `ROTATION=1` (ландшафт 160×128) |
| USB-мост | CH343 (`VID_1A86:PID_55D3`) → UART0 |
| Управление | 3 тактовые кнопки на GPIO |

## Карта контактов

### Дисплей ST7735S

Отдельная SPI-шина дисплея, не пересекается с шиной радиомодулей.

| Сигнал дисплея | Подпись на плате | GPIO |
|---|---|---|
| `MOSI` / `SDA` | Display SDA | **GPIO11** |
| `SCLK` / `SCL` | Display SCL | **GPIO12** |
| `CS` | Display CS | **GPIO46** |
| `DC` / `RS` / `A0` | Display DC | **GPIO9** |
| `RES` / `RST` | Display RES | **GPIO10** |
| `LED` / `BLK` | Display BLK | **GPIO3** |
| `VCC` | — | 3.3 V, не 5 V |
| `GND` | — | GND |

> GPIO3 — strapping-пин (выбор источника загрузки). Обычно не мешает, но при
> проблемах со входом в download mode удерживайте BOOT при сбросе.

### Три кнопки

| Кнопка | Подпись на плате | GPIO | Схема подключения | Функция |
|---|---|---|---|---|
| **UP** | `LEFT` | **GPIO21** | GPIO21 → кнопка → GND | вверх / назад по меню |
| **SEL** | `SELECT` | **GPIO48** | GPIO48 → кнопка → GND | ОК; долгое нажатие = ESC/назад |
| **DOWN** | `RIGHT` | **GPIO47** | GPIO47 → кнопка → GND | вниз / вперёд по меню |

Внутренние подтяжки включаются в `_setup_gpio()`, внешние резисторы **не нужны**.
Подробности — [docs/BUTTONS.md](docs/BUTTONS.md).

### Шина CC1101 (Sub-GHz)

| Сигнал | Подпись на плате | GPIO |
|---|---|---|
| `MISO` | CC1101 MISO | **GPIO8** |
| `MOSI` | CC1101 MOSI | **GPIO17** |
| `SCK` | CC1101 SCK | **GPIO18** |
| `CSN` / `SS` | CC1101 CSN | **GPIO15** |
| `GDO0` | CC1101 GDO0 | **GPIO16** |
| `GDO2` | CC1101 GDO2 | **GPIO6** |

### Прочие узлы платы

| Узел | Подпись на плате | GPIO |
|---|---|---|
| SD-карта SPI | SD MISO / SCK / MOSI / CS | 19 / 20 / 41 / 42 |
| I²C (PN532) | SCL / SDA | 1 / 2 |
| IR | IR TX / IR RX | 5 / 4 |
| GPS | ESP TX / ESP RX | 40 / 39 |
| Энкадер | A / B | 14 / 13 (в профиле не используется) |
| Логический UART | TXD0 / RXD0 | 43 / 44 |

### Занятые и недоступные GPIO

Занято платными узлами: **1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 15, 16, 17, 18, 19,
20, 21, 40, 41, 42, 43, 44, 46, 47, 48**.

> **Не использовать GPIO26–GPIO32** — SPI flash 16 МБ.
> **Не использовать GPIO33–GPIO37** — octal PSRAM (N16**R8**).
> Именно поэтому **NRF24 в этом профиле выключен**: штатные ноги nRF24 у таких
> плат попадают на GPIO35–38, то есть на PSRAM.
> **GPIO0, GPIO3, GPIO45, GPIO46** — strapping-пины.
> **GPIO19/GPIO20** — ножки native USB; USB отключён (`ARDUINO_USB_MODE=0`),
> поэтому ноги отданы SD-SPI.

---

## Быстрый старт

```bash
# 1. Клонируем Bruce и подкладываем порт
git clone --depth 1 https://github.com/BruceDevices/Bruce.git firmware
cp boards/esp32s3-n16r8-st7735s.ini          firmware/boards/
cp boards/_boards_json/esp32s3-n16r8-st7735s.json firmware/boards/_boards_json/
cp -r boards/esp32s3-n16r8-st7735s           firmware/boards/
cp -r boards/esp32s3-n16r8-st7735s_board     firmware/boards/

# 2. Собираем
cd firmware
pio run -e esp32s3-n16r8-st7735s

# 3. Прошиваем (плату нужно перевести в download mode, см. ниже)
pio run -e esp32s3-n16r8-st7735s -t upload --upload-port COM3
```

Windows-версии тех же команд — в [docs/INSTALL.md](docs/INSTALL.md).

### Перевод в download mode

На плате **нет схемы авто-сброса**, поэтому перед каждой прошивкой:

1. зажать **BOOT** (GPIO0);
2. нажать и отпустить **RESET** (EN);
3. отпустить **BOOT**.

После этого `esptool` увидит чип. Подробно — [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md).

---

## Что не вошло в порт

Осознанные ограничения профиля:

* **Нет датчика батареи** — `getBattery()` возвращает 0, индикатор заряда не показывается.
* **Нет управления питанием** — `powerOff()` и глубокий сон не реализованы.
* **Регулировка яркости — no-op.** Подсветка жёстко включена на GPIO3.
* **LTO выключен** (`build_unflags = -flto`) — иначе линковка падает с
  `SCons.Tool.FortranCommon`. Прошивка получается крупнее примерно на 1–2 %.
* **Native USB (BadUSB) отключён**: `ARDUINO_USB_MODE=0`, `Serial` выведен на UART0 (CH343),
  чтобы видеть логи на COM-порту.

Детали и способы переопределить — [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md).

---

## Размер прошивки

```
RAM:   37 % (121 KB из 320 KB)
Flash: 25 % (4.2 MB из 16 MB)
```

---

## Лицензия

Порт распространяется под лицензией MIT — см. [LICENSE](LICENSE).
Bruce Firmware и используемые библиотеки имеют собственные лицензии.
