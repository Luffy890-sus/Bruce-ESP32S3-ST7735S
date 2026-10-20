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

## Готовая конфигурация (прошита и проверена)

| Узел | Пины | Статус |
|---|---|---|
| Дисплей ST7735S 128×160 | CS=8, DC=9, RST=15, MOSI=17, SCLK=18, BL=21 | работает |
| 3 кнопки | UP=5, SEL=4, DOWN=6 | работает |
| CC1101 Sub-GHz | CSN=12, GDO0=14, GDO2=47 + общая SPI (17/18/19) | включён в прошивку |
| Питание | внешний LiPo 3.7 V → TP4056 → Boost 3.3 V | см. инструкцию |
| Логи | UART0 (GPIO43/44) → CH343 → COM-порт, 115200 | работает |
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

| Сигнал дисплея | GPIO | Примечание |
|---|---|---|
| `CS` | **GPIO8** | chip select |
| `DC` | **GPIO9** | data/command |
| `RST` | **GPIO15** | сброс дисплея |
| `MOSI` | **GPIO17** | SPI MOSI |
| `SCLK` | **GPIO18** | SPI SCK |
| `BL` | **GPIO21** | подсветка |
| `VCC` | 3.3 V | не 5 V |
| `GND` | GND | |

### Три кнопки

| Кнопка | GPIO | Схема подключения | Функция |
|---|---|---|---|
| **UP** | **GPIO5** | GPIO5 → кнопка → GND | вверх / назад по меню |
| **SEL** | **GPIO4** | GPIO4 → кнопка → GND | ОК; долгое нажатие = ESC/назад |
| **DOWN** | **GPIO6** | GPIO6 → кнопка → GND | вниз / вперёд по меню |

Внутренние подтяжки включаются в `_setup_gpio()`, внешние резисторы **не нужны**.
Подробности — [docs/BUTTONS.md](docs/BUTTONS.md).

### Общая шина SPI

| Сигнал | GPIO |
|---|---|
| `SCK` | GPIO18 |
| `MOSI` | GPIO17 |
| `MISO` | GPIO19 |
| `SS` | GPIO10 |

### Свободные GPIO

Доступны для внешних модулей: **GPIO1, GPIO2, GPIO7, GPIO10, GPIO11, GPIO13,
GPIO22, GPIO38, GPIO39, GPIO40, GPIO41, GPIO42**.

Занято: **4/5/6** — кнопки, **8/9/15/17/18/21** — дисплей, **12/14/47** — CC1101,
**19** — SPI MISO, **13** — I2C SDA, **43/44** — логи, **48** — RGB LED.

> **Не использовать GPIO26–GPIO37** — они заняты flash и octal PSRAM (N16**R8**).
> **GPIO0, GPIO3, GPIO45, GPIO46** — strapping-пины, трогать осторожно.
> **GPIO19/GPIO20** — ножки native USB; GPIO19 уже занят под `MISO`.

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
* **Регулировка яркости — no-op.** Подсветка жёстко включена на GPIO21.
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
