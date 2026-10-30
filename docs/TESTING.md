# Testing checklist

Record actual results here before describing the project as validated.

## Test setup

- Board and revision:
- Arduino IDE version:
- Sensor model:
- Driver model:
- Soil / test medium:
- Date:

## Tests

- [ ] Sketch compiles for the selected board.
- [ ] Serial Monitor reports stable raw readings.
- [ ] Dry and wet calibration values have been measured.
- [ ] Moisture percentage moves in the expected direction.
- [ ] Pump output stays OFF when `PUMP_ENABLED = false`.
- [ ] Driver input behavior is confirmed with the pump disconnected.
- [ ] Maximum-runtime cutoff has been checked safely.
- [ ] Low-voltage pump test completed under supervision, if applicable.

## Results and observations

Add measured readings, problems, and fixes here. Do not mark tests as passed until you have run them on real hardware.
