# Типовые проблемы

Собрано из реальных ошибок, воспроизведённых на этой плате.

---

## `core/powerSave.h: No such file or directory`

```
boards/esp32s3-n16r8-st7735s/interface.cpp:1:10: fatal error: core/powerSave.h:
No such file or directory
```

**Причина.** `interface.cpp` лежит в variant-папке `boards/esp32s3-n16r8-st7735s/`.
Arduino Framework компилирует variant **до** основного исходника и не видит
include-пути прошивки (`src/core`, `include/`).

**Лечение.** Держать `interface.cpp` в отдельной папке
`boards/esp32s3-n16r8-st7735s_board/`, а в `boards/esp32s3-n16r8-st7735s/`
оставить только `pins_arduino.h`. Проверьте `build_src_filter`:

```ini
build_src_filter = ${env.build_src_filter} \
    -<../boards/ESP-General/interface.cpp> \
    +<../boards/esp32s3-n16r8-st7735s_board>
```

---

## Массовые ошибки компиляции в `fl/fx/…`, `fl/math/…`, `fl/stl/json.h`

Выглядит как тысячи ошибок внутри FastLED, вида
`expected ';' before 'FP'`, `‘FP’ does not name a type`.

**Причина.** `#define FP 1` (размер шрифта) конфликтует с FastLED, где `FP` —
alias типа `fl::s16x16`.

**Лечение.** Не определять `FP`/`FM`/`FG` в `pins_arduino.h`. В Bruce они уже
есть в `precompiler_flags.h` под `#ifndef`. Удалите эти три строки из
`pins_arduino.h`, если скопировали их из `ESP-General.ini`.

---

## `ModuleNotFoundError: No module named 'SCons.Tool.FortranCommon'`

Возникает на этапе линковки `firmware.elf` — то есть **в самом конце** сборки,
когда включён LTO:

```
*** [.pio/build/esp32s3-n16r8-st7735s/firmware.elf] ModuleNotFoundError :
    No module named 'SCons.Tool.FortranCommon'
========================= [FAILED] =========================
```

**Причина.** Bruce включает LTO флагом `-flto` в `platformio.ini`
(`[env] build_flags`). LTO-проход линковки дёргает из SCons инструмент
`fortran`, а в том SCons, который реально импортируется при сборке, модуля
`FortranCommon` нет. Сам файл при этом может лежать в
`~/.platformio/packages/tool-scons/scons-local-*/SCons/Tool/` — Python его
просто не оттуда грузит.

**Лечение (правильное).** Убрать `-flto`, чтобы линковка вообще не входила
в LTO-режим. В `boards/esp32s3-n16r8-st7735s.ini`:

```ini
build_unflags = -flto
```

Именно это и сделано в этом порту. Цена — прошивка примерно на 1–2 % больше.

Важно: **`build_unflags`, а не «дописать `-fno-lto` в `build_flags`»**.
Второй вариант работает нестабильно — флаг попадает только в `CCFLAGS`, а
`-flto` в `LINKFLAGS` может остаться, и на инкрементальной сборке ошибка
возвращается.

После правки нужна **чистая** пересборка:

```bash
rm -rf .pio/build/esp32s3-n16r8-st7735s
pio run -e esp32s3-n16r8-st7735s
```

**Лечение (хрупкое).** Доустановить SCons в venv PlatformIO:

```bash
~/.platformio/penv/bin/pip install "scons==4.11.1"
```

Минус: `pio run -t upload` периодически переустанавливает `tool-scons`
в выводе мелькает `Tool Manager: Installing ... scons-local-*.tar.gz` — и
ошибка возвращается.

---

## `cannot find -lnet80211`

```
xtensa-esp32s3-elf-gcc: error: cannot find -lnet80211: No such file or directory
```

**Причина.** Скрипт `patch.py` в Bruce переименовывает
`libnet80211.a` → `libnet80211.a.old`, чтобы вырезать конфликтующий символ,
но патченную версию не создаёт. При этом `-lnet80211` остаётся в `build_flags`.

**Лечение.** Убрать флаг `-lnet80211` из `build_flags` окружения и вернуть
библиотеку на место:

```bash
cd ~/.platformio/packages/framework-arduinoespressif32-libs/esp32s3/esp-idf
mv libnet80211.a.old libnet80211.a
```

---

## `multiple definition of ieee80211_raw_frame_sanity_check`

**Причина.** Символ определён и в `libnet80211.a`, и в патченной обёртке Bruce.

**Лечение.** Ослабить символ в архиве:

```bash
xtensa-esp32s3-elf-objcopy --weaken-symbol=ieee80211_raw_frame_sanity_check \
  libnet80211.a
```

Если после `pio run -t clean` ошибка возвращается вместе с `FortranCommon` —
смотрите два предыдущих пункта, они связаны.

---

## `Failed to connect to ESP32-S3: No serial data received`

Самая частая проблема именно этой платы.

**Причина.** На плате **нет RC-цепочки авто-сброса** на EN и GPIO0.
Первая прошивка проходит, потому что чип ещё в download mode с завода;
все последующие — нет.

**Лечение.** Вручную:

1. зажать **BOOT** (GPIO0);
2. нажать и отпустить **RESET** (EN);
3. отпустить **BOOT**;
4. сразу запустить прошивку.

Проверка, что чип отвечает:

```bash
esptool --chip esp32s3 --port COM3 --baud 115200 chip-id
```

Хорошо, если вывод начинается так:

```
Connecting....
Chip type:          ESP32-S3 (QFN56) (revision v0.2)
Features:           Wi-Fi, BT 5 (LE), Dual Core + LP Core, 240MHz, Embedded PSRAM 8MB (AP_3v3)
```

Строка `Embedded PSRAM 8MB` подтверждает, что это именно N16**R8**.

Дополнительно:

* кабель **обязательно** data-кабель, не «для зарядки»;
* на 460800 baud мост CH343 иногда не тянет — попробуйте `--baud 115200`;
* закройте монитор порта, он держит COM;
* Linux: добавить пользователя в группу `dialout`:
  `sudo usermod -aG dialout $USER` (перелогиниться).

---

## COM-порт есть, но логи пустые

Скорее всего прошивка собрана с `ARDUINO_USB_MODE=1` — тогда `Serial`
уходит в native USB, а не в UART0/CH343.

**Диагноз.** Посмотреть список устройств:

```powershell
Get-WMIObject Win32_PnPEntity | Where-Object { $_.Name -match "COM\d" }
```

Если есть только `CH343 (COMx)` и нет `VID_303A` — логи идут в UART0, и
нужно собрать с `ARDUINO_USB_MODE=0` (как в этом порте по умолчанию).

---

## `Brownout detector was triggered`

Просадка питания при старте Wi-Fi/Bluetooth.

* питание **только 3.3 V**, не 5 V на ножку 3V3;
* конденсатор 100–220 мкФ между 3V3 и GND рядом с платой;
* короче провода, толще (≥ 0.75 мм²);
* отключить подсветку на время отладки (`-DTFT_BL=-1`).

---

## Экран белый / инвертированные цвета / смещено изображение

| Симптом | Причина | Что менять |
|---|---|---|
| Белый экран, подсветка есть | нет инициализации | проверить `RST`=GPIO10, `CS`=GPIO46, `DC`=GPIO9, `MOSI`=11, `SCLK`=12, `BLK`=3 |
| Синий ↔ красный | неверный порядок каналов | `-DTFT_RGB_ORDER=TFT_RGB` |
| Изображение сдвинуто на 1–2 пикселя | другой вариант панели | `-DST7735_BLACKTAB` или `-DST7735_GREENTAB` |
| Тёмное/белое инвертировано | инверсия | `-DTFT_INVERTED` |
| Неправильная ориентация | поворот | `-DROTATION=0/1/2/3` |

---

## `*** WARNING: Windows Long Path Support is disabled ***`

Не ошибка, но на глубоком дереве `.pio` пути могут превышать 260 символов,
что даёт странные «file not found».

```powershell
# от администратора
New-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem' `
  -Name 'LongPathsEnabled' -Value 1 -PropertyType DWORD -Force
# перезагрузка
```

---

## Плата «оживает» только после нескольких RESET

Обычно это strapping: GPIO0 или GPIO46 подтянуты чем-то вниз.
Убедитесь, что на GPIO0/3/45/46 ничего не висит, кроме штатных цепей.
