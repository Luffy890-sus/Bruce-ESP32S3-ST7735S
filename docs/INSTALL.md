# Установка, сборка, прошивка

## 0. Требования

| Компонент | Версия | Зачем |
|---|---|---|
| Python | 3.10–3.12 | PlatformIO |
| PlatformIO Core | ≥ 6.1 | сборка |
| platform-espressif32 | **55.03.39** (Arduino core 3.3.9) | задаётся в `platformio.ini` Bruce, ставить отдельно не нужно |
| Драйвер CH343 | любой актуальный | Windows обычно ставит сам |

Проверка:

```bash
pio --version          # PlatformIO Core 6.1.x
```

Если PlatformIO нет:

```bash
python -m pip install -U platformio
```

---

## 1. Получить исходники Bruce

```bash
git clone --depth 1 https://github.com/BruceDevices/Bruce.git firmware
cd firmware
```

Первая сборка скачает платформа (~1 ГБ) и библиотеки. Это нормально.

---

## 2. Положить файлы порта в дерево Bruce

Из корня этого репозитория:

```bash
cp boards/esp32s3-n16r8-st7735s.ini              firmware/boards/
cp boards/_boards_json/esp32s3-n16r8-st7735s.json firmware/boards/_boards_json/
cp -r boards/esp32s3-n16r8-st7735s               firmware/boards/
cp -r boards/esp32s3-n16r8-st7735s_board         firmware/boards/
```

PowerShell:

```powershell
Copy-Item boards/esp32s3-n16r8-st7735s.ini              ../firmware/boards/
Copy-Item boards/_boards_json/esp32s3-n16r8-st7735s.json ../firmware/boards/_boards_json/
Copy-Item -Recurse boards/esp32s3-n16r8-st7735s          ../firmware/boards/
Copy-Item -Recurse boards/esp32s3-n16r8-st7735s_board    ../firmware/boards/
```

Что куда:

| Файл в этом репо | Куда в Bruce | Назначение |
|---|---|---|
| `boards/esp32s3-n16r8-st7735s.ini` | `firmware/boards/` | конфигурация окружения сборки |
| `boards/_boards_json/esp32s3-n16r8-st7735s.json` | `firmware/boards/_boards_json/` | описание платы (подхватывается через `boards_dir`) |
| `boards/esp32s3-n16r8-st7735s/pins_arduino.h` | `firmware/boards/esp32s3-n16r8-st7735s/` | variant-заголовки Arduino (`board_build.variants_dir = boards`) |
| `boards/esp32s3-n16r8-st7735s_board/interface.cpp` | `firmware/boards/esp32s3-n16r8-st7735s_board/` | профиль платы: GPIO, кнопки, батарея |

> **Важно про имя папки.** `interface.cpp` лежит в папке `esp32s3-n16r8-st7735s_board`,
> а не в `esp32s3-n16r8-st7735s`. Это не опечатка: папка `esp32s3-n16r8-st7735s`
> обрабатывается Arduino-framework как *variant* (туда входит только `pins_arduino.h`),
> а `esp32s3-n16r8-st7735s_board` подключается через `build_src_filter` как обычный
> исходник прошивки, где доступны заголовки `core/…` и `include/…`.
> Если положить `interface.cpp` в variant-папку, сборка упадёт с
> `core/powerSave.h: No such file or directory`.

Проверка, что Bruce видит окружение:

```bash
cd firmware
pio run -e esp32s3-n16r8-st7735s -t env
```

В выводе должны быть строки `PLATFORM: Espressif 32 (55.3.39) > ESP32-S3-N16R8 with ST7735S`.

---

## 3. Сборка

```bash
cd firmware
pio run -e esp32s3-n16r8-st7735s
```

Windows:

```powershell
cd firmware
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e esp32s3-n16r8-st7735s
```

Ориентировочное время чистой сборки: **25–30 минут** на обычном ПК
(компилируются RadioLib, IRremoteESP8266, NimBLE и прочее).
Инкрементальная — 1–3 минуты.

Ожидаемый результат:

```
RAM:   [====      ]  36.9% (used 121036 bytes from 327680 bytes)
Flash: [===       ]  25.2% (used 4221415 bytes from 16777216 bytes)
======================== [SUCCESS] ========================
```

Артефакты:

| Файл | Что это |
|---|---|
| `.pio/build/esp32s3-n16r8-st7735s/firmware.bin` | приложение, пишется в `0x10000` |
| `.pio/build/esp32s3-n16r8-st7735s/bootloader.bin` | `0x0` |
| `.pio/build/esp32s3-n16r8-st7735s/partitions.bin` | `0x8000` |
| `Bruce-esp32s3-n16r8-st7735s.bin` | factory-образ целиком, пишется в `0x0` |

### Карта разделов (`custom_16Mb.csv`)

```
# Name,     Type, SubType,   Offset,   Size,    Flags
nvs,        data, nvs,       0x9000,   0x6000,
factory,    app,  factory,   0x10000,  0x470000,
spiffs,     data, spiffs,    0x480000, 0xB70000,
coredump,   data, coredump,  0xFF0000, 0x10000,
```

~4.4 МБ под прошивку, ~11.4 МБ под хранилище (скрипты, словари, пилы).

---

## 4. Прошивка

### Подготовка платы

На плате **нет авто-сброса**, поэтому перед каждой прошивкой:

1. зажать **BOOT** (GPIO0);
2. нажать и отпустить **RESET** (EN);
3. отпустить **BOOT**.

Узнать номер порта:

```powershell
[System.IO.Ports.SerialPort]::GetPortNames()      # Windows
ls /dev/ttyUSB* /dev/ttyACM*                      # Linux
```

### Вариант A — PlatformIO

```bash
pio run -e esp32s3-n16r8-st7735s -t upload --upload-port COM3
```

### Вариант B — esptool напрямую

```bash
esptool --chip esp32s3 --port COM3 --baud 460800 write_flash \
  0x0     .pio/build/esp32s3-n16r8-st7735s/bootloader.bin \
  0x8000  .pio/build/esp32s3-n16r8-st7735s/partitions.bin \
  0xe000  ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin \
  0x10000 .pio/build/esp32s3-n16r8-st7735s/firmware.bin
```

### Вариант C — одним factory-образом

```bash
esptool --chip esp32s3 --port COM3 --baud 460800 write_flash 0x0 Bruce-esp32s3-n16r8-st7735s.bin
```

### Мониторинг логов

```bash
pio device monitor -p COM3 -b 115200
```

`Serial` в этом порту выведен на **UART0** (GPIO43/44 → CH343), 115200 8N1.

---

## 5. Первый запуск

1. После прошивки плата перезагрузится сама (esptool дёргает RTS).
2. На ST7735S появится boot-логотип Bruce, затем главное меню.
3. Прокрутка — `DOWN`/`UP`, выбор — `SEL`, назад — долгое `SEL`.
4. Веб-интерфейс по умолчанию поднимается как startup-приложение
   (`bruceConfig.startupApp = "WebUI"` в `_setup_gpio()`).
   Отключается в `Settings → Startup App`.
5. SD-карты нет (`SDCARD_CS = -1`), хранилище — LittleFS/spiffs в flash.

---

## 6. Что делать при ошибках

См. [TROUBLESHOOTING.md](TROUBLESHOOTING.md).
