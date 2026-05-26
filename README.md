# TinyTemp — ATtiny85 Digital Thermometer with OLED Display

A battery-powered digital thermometer built on an ATtiny85 microcontroller. Reads temperature via a TMP-36 analog sensor, averages 25 ADC samples to reduce noise, and displays the result on an SSD1306 OLED. The microcontroller sleeps for 8-second intervals using the watchdog timer to conserve battery between readings.

[Demo video →](https://youtube.com/your-demo-link-here) <!-- replace with your YouTube link -->

## Hardware

| Component | Notes |
|---|---|
| ATtiny85 | 8-pin DIP, runs at 1 MHz (internal oscillator) |
| TMP-36 | Analog temperature sensor, connected to ADC pin (PB4) |
| SSD1306 OLED | 128×32, I2C address 0x3C |
| Power | 3xAAA (4.5V) or coin cell via 3.3V regulator |

## Key Technical Details

- **Bit-banged I2C** — ATtiny85 has no hardware I2C peripheral, so `tinyOLED.c` implements the full I2C protocol in software (SCL on PB2, SDA on PB0)
- **ADC averaging** — 25 samples per reading with a settling delay to reduce noise from the low-voltage supply
- **Watchdog sleep** — `tinyTempSleep.c` uses the AVR watchdog timer in interrupt mode for 8-second sleep cycles, keeping current draw well under 1 µA during sleep
- **Custom OLED driver** — `tinyOLED.c/h` provides a minimal SSD1306 driver that fits in ATtiny85's 8 KB flash, based on Stefan Wagner's [tinyOLED](https://github.com/wagiminator/ATtiny13-TinyOLEDdemo) library

## Files

| File | Description |
|---|---|
| `TinyTemp/tinyTempSleep.c` | Main firmware — ADC read, temperature conversion, display update, watchdog sleep |
| `TinyTemp/tinyTempSleep.h` | Header for sleep/temperature functions |
| `TinyTemp/tinyOLED.c` | Bit-banged I2C + SSD1306 OLED driver |
| `TinyTemp/tinyOLED.h` | OLED driver API |
| `TinyTemp/makefile` | Build and flash configuration |
| `TinyTemp/main.hex` | Pre-compiled firmware — flash directly without a toolchain |

## Build & Flash

**Requirements:** `avr-gcc`, `avr-libc`, `avrdude`, a USBasp or similar AVR programmer

```bash
cd TinyTemp
make          # compiles to main.hex
make flash    # flashes via avrdude (USBasp)
```

Or flash the pre-compiled `main.hex` directly:
```bash
avrdude -c usbasp -p attiny85 -U flash:w:main.hex:i
```

## Temperature Formula

```c
// TMP-36: 10 mV/°C, 500 mV at 0°C
// ADC reference = Vcc (~4.5V for 3xAAA)
float voltage = (adc_avg / 1024.0) * VCC_MV;
float temp_c  = (voltage - 500.0) / 10.0;
```

## Attribution

- `tinyOLED.c/h` based on [Stefan Wagner's tinyOLED library](https://github.com/wagiminator/ATtiny13-TinyOLEDdemo) — adapted for ATtiny85 and extended with character rendering
- Makefile template from ECE-304 (Prof. McLaughlin, UMass Amherst)
- Co-developed with Robert K. (ECE-304, UMass Amherst)
