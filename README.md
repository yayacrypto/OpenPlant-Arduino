# OpenPlant Arduino

An open-source, beginner-friendly plant monitoring and low-voltage watering controller for Arduino.

OpenPlant reads a soil-moisture sensor, reports readings over Serial, and can switch a **low-voltage** pump through a compatible relay or MOSFET driver when the soil becomes too dry. It uses separate ON/OFF thresholds (hysteresis) to reduce rapid switching and includes a maximum pump-run timer as a basic safety limit.

> **Project status:** Initial prototype / community contribution welcome. This repository has not yet been validated across all boards and sensor models. Test with the pump disconnected first.

## Features

- Reads an analog soil-moisture sensor.
- Converts raw readings into an approximate 0–100% moisture scale using calibration values.
- Uses hysteresis to avoid rapid pump toggling.
- Stops watering after a configurable maximum run time.
- Supports a manual pump-disable setting in the sketch.
- Prints readable diagnostics over Serial.
- No cloud account or paid service required.

## Hardware

- Arduino Uno or compatible board
- Capacitive soil-moisture sensor with analog output (recommended over resistive probes)
- Optional low-voltage DC pump
- Compatible relay module or logic-level MOSFET driver suitable for the pump
- Suitable external power supply for the pump
- Jumper wires and breadboard as appropriate

**Electrical safety:** Keep the prototype low-voltage. Do not connect mains electricity to a breadboard or follow this project as a mains-wiring guide. A pump needs its own appropriately rated power supply; do not power it directly from an Arduino I/O pin. Check the driver module's ratings and wiring documentation.

## Wiring overview

| Part | Arduino connection |
|---|---|
| Sensor analog output | `A0` |
| Sensor VCC | Board-compatible supply, according to sensor documentation |
| Sensor GND | GND |
| Driver control input | `D7` |
| Status LED | Built-in LED (`LED_BUILTIN`, if available) |

Connect the pump to its correctly rated low-voltage driver and power supply according to the driver's documentation. Share ground only where required by the driver design. **Do not assume every relay module is active-high**; configure `PUMP_ACTIVE_LOW` below.

## Setup

1. Open `firmware/OpenPlant.ino` in the Arduino IDE.
2. Select your board and port.
3. Upload the sketch with the pump disconnected.
4. Open Serial Monitor at **9600 baud** and observe the raw sensor reading.
5. With the sensor in the intended soil, note readings for dry soil and thoroughly watered soil.
6. Update `DRY_RAW` and `WET_RAW` in the sketch to match your sensor. On many sensors, the raw value may increase or decrease as soil gets wetter, so check your own readings.
7. Keep `PUMP_ENABLED` set to `false` while calibrating.
8. After calibration, test the driver without a pump first. Then, if appropriate, test with a low-voltage pump while supervised.

## Configuration

These settings are near the top of `firmware/OpenPlant.ino`:

- `DRY_RAW`: calibrated raw reading for dry soil
- `WET_RAW`: calibrated raw reading for wet soil
- `WATER_ON_BELOW_PERCENT`: start watering below this moisture percentage
- `WATER_OFF_ABOVE_PERCENT`: stop watering above this percentage
- `MAX_PUMP_RUNTIME_MS`: maximum continuous pump-on duration
- `SOAK_DELAY_MS`: pause after a watering run before evaluating again
- `PUMP_ENABLED`: keep `false` to disable pump output during calibration
- `PUMP_ACTIVE_LOW`: set to `true` for active-low driver inputs

The percentage is an approximate calibrated scale, not a scientific measurement of water content. Different sensors and soils require calibration.

## Serial output

Example format:

```text
raw=612 moisture=34% pump=OFF
raw=590 moisture=38% pump=OFF
```

Values shown above are illustrative only.

## Project structure

```text
OpenPlant-Arduino/
├── firmware/
│   └── OpenPlant.ino
├── docs/
│   └── TESTING.md
├── .github/
│   ├── ISSUE_TEMPLATE/
│   │   ├── bug_report.md
│   │   └── feature_request.md
│   └── PULL_REQUEST_TEMPLATE.md
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── LICENSE
└── README.md
```

## Roadmap

- [ ] Validate calibration with multiple sensor models
- [ ] Add a non-blocking watering scheduler
- [ ] Add optional OLED display support
- [ ] Add documented test results from different boards
- [ ] Add translations for the documentation

## Contributing

Bug reports, clearer documentation, calibration examples, and tested improvements are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md). Please include your board, sensor model, calibration readings, and steps to reproduce any issue.

## License

MIT. See [LICENSE](LICENSE).
