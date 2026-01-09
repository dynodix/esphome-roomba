# ESPHome Roomba External Component

An **ESPHome external component** for controlling and monitoring legacy **iRobot Roomba** vacuum cleaners via their **SCI (Serial Command Interface)**.

This project replaces deprecated `custom_component`-based integrations and is fully compatible with **ESPHome 2025.x+**, including **Home Assistant Green**.

It has been tested with **ESP32-S3 DevKitC-1** and Roomba models from the **4xx / 5xx series**.

---

## Features

### Control
- Start / Stop cleaning
- Dock (return to base)
- Spot cleaning
- Locate (play sound)
- Wake Roomba via BRC pin

### Sensors
- Distance traveled
- Battery voltage
- Battery current
- Battery charge
- Battery capacity
- Battery percentage
- Battery temperature
- Activity state:
  - Cleaning
  - Docked
  - Charging
  - Lost

---

## Wiring (Text Schematic)

### Roomba SCI Mini-DIN 7 (male, notch up)

```
        _________
       /         \
      |   6   7   |
      | 4   ▢   5 |
      |   2   3   |
       \____1____/
```

| Pin | Signal | Description |
|----:|--------|-------------|
| 1 | GND | Ground |
| 2 | RX | Data to Roomba |
| 3 | TX | Data from Roomba |
| 5 | BRC | Wake / baud control |
| 6 | +5V | 5V power output |

---

### ESP32-S3 DevKitC-1 Wiring

| Roomba Pin | Signal | ESP32-S3 Pin |
|-----------:|--------|--------------|
| 6 | +5V | 5V / VIN |
| 1 | GND | GND |
| 3 | TX | GPIO 8 (RX) |
| 2 | RX | GPIO 7 (TX) |
| 5 | BRC | GPIO 9 |

---

### ASCII Wiring Diagram

```
ESP32-S3 DevKitC-1                 Roomba SCI
------------------                 ----------
5V / VIN  <----------------------  +5V (Pin 6)
GND       <----------------------  GND (Pin 1)
GPIO 8 RX <----------------------  TX  (Pin 3)
GPIO 7 TX ---------------------->  RX  (Pin 2)
GPIO 9    ---------------------->  BRC (Pin 5)
```

---

## Power Notes (IMPORTANT)

The ESP32-S3 DevKit board **may be powered from 5V**, but Wi-Fi current peaks can cause brownouts.

**Strongly recommended:**
- 470–1000 µF electrolytic capacitor between 5V and GND
- 0.1 µF ceramic decoupling capacitor
- Short wires and solid ground

If instability occurs, power the ESP32 via USB instead.

---

## Disclaimer

Not affiliated with iRobot.  
Use at your own risk.
