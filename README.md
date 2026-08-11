# ESPHome Roomba External Component

An **ESPHome external component** for controlling and monitoring legacy **iRobot Roomba** vacuum cleaners via their **SCI / Open Interface serial connection**.

The component is intended as a replacement for older `custom_component`-based ESPHome integrations. It has been tested with an **ESP32-S3 DevKitC-1**, **ESPHome 2026.7.x**, and legacy Roomba models from the **4xx / 5xx series**.

## Features

### Control
- Start / Stop cleaning
- Dock / return to base
- Spot cleaning
- Locate by playing a short sound
- Wake Roomba using the BRC pin

### Sensors
- Distance travelled
- Battery voltage
- Battery current
- Battery charge
- Battery capacity
- Battery percentage
- Battery temperature
- Activity state: Cleaning, Docked, Charging or Lost

## Installation

The recommended installation method is to load the component directly from this GitHub repository.

```yaml
esphome:
  name: esp-roomba
  libraries:
    - EspSoftwareSerial

external_components:
  - source:
      type: git
      url: https://github.com/dynodix/esphome-roomba
      ref: main
    components:
      - roomba
    refresh: 0s
```

`refresh: 0s` is useful while developing or testing because ESPHome checks the repository on every build. Once the configuration is stable, it may be omitted to allow the normal external-component cache behaviour.

The old dependency below is **not required and should not be used**:

```yaml
- Roomba=https://github.com/davidecavestro/Roomba.git
```

The required Roomba Open Interface wrapper is now included directly in `components/roomba/` as `Roomba.h` and `Roomba.cpp`. `EspSoftwareSerial` remains the only external Arduino library required by this component.

A complete working example is available in [`esp-roomba.yaml`](esp-roomba.yaml).

## Component configuration

```yaml
roomba:
  id: roomba1
  brc_pin: 9
  rx_pin: 8
  tx_pin: 7
  baud: 115200
  update_interval: 30s
```

Example sensor configuration:

```yaml
sensor:
  - platform: roomba
    id: roomba1
    distance:
      name: "Roomba distance"
    voltage:
      name: "Roomba voltage"
    current:
      name: "Roomba current"
    charge:
      name: "Roomba charge"
    capacity:
      name: "Roomba capacity"
    battery:
      name: "Roomba battery"
    temperature:
      name: "Roomba temperature"

text_sensor:
  - platform: roomba
    id: roomba1
    activity:
      name: "Roomba activity"
```

Example buttons:

```yaml
button:
  - platform: template
    name: "Locate"
    on_press:
      - lambda: |-
          id(roomba1).on_command("locate");

  - platform: template
    name: "Start/Stop"
    on_press:
      - lambda: |-
          id(roomba1).on_command("start");

  - platform: template
    name: "Dock"
    on_press:
      - lambda: |-
          id(roomba1).on_command("dock");
```

## Wiring

### Roomba SCI Mini-DIN 7

With the male connector viewed with the notch up:

```text
        _________
       /         \
      |   6   7   |
      | 4   ▢   5 |
      |   2   3   |
       \____1____/
```

| Roomba pin | Signal | Description |
|---:|---|---|
| 1 | GND | Ground |
| 2 | RX | Data to Roomba |
| 3 | TX | Data from Roomba |
| 5 | BRC | Wake / baud control |
| 6 | +5V | 5 V power output |

### ESP32-S3 DevKitC-1

| Roomba pin | Signal | ESP32-S3 pin |
|---:|---|---|
| 6 | +5V | 5V / VIN |
| 1 | GND | GND |
| 3 | TX | GPIO 8 (RX) |
| 2 | RX | GPIO 7 (TX) |
| 5 | BRC | GPIO 9 |

```text
ESP32-S3 DevKitC-1                 Roomba SCI
------------------                 ----------
5V / VIN  <----------------------  +5V (Pin 6)
GND       <----------------------  GND (Pin 1)
GPIO 8 RX <----------------------  TX  (Pin 3)
GPIO 7 TX ---------------------->  RX  (Pin 2)
GPIO 9    ---------------------->  BRC (Pin 5)
```

## Power notes

The ESP32-S3 development board may be powered from the Roomba 5 V supply, but Wi-Fi current peaks can cause brownouts.

Recommended:
- 470–1000 µF electrolytic capacitor between 5 V and GND
- 0.1 µF ceramic decoupling capacitor
- short wiring and a solid common ground

If instability occurs, power the ESP32 from USB or another suitable regulated supply while keeping the grounds common.

## Architecture

The project consists of:

- `components/roomba/__init__.py` — ESPHome configuration schema and code generation
- `components/roomba/sensor.py` — numeric sensor integration
- `components/roomba/text_sensor.py` — activity text sensor
- `components/roomba/roomba_component.cpp/.h` — ESPHome component logic
- `components/roomba/Roomba.cpp/.h` — embedded minimal Roomba Open Interface wrapper

This layout avoids the PlatformIO manifest problem of the historical Arduino Roomba repository and allows current ESPHome releases to build the component without separately downloading that library.

## Compatibility

- Tested with Roomba 560
- Expected to work with many legacy 4xx / 5xx series Roombas using the SCI/Open Interface connector
- Not intended for newer Wi-Fi-only Roomba models without the compatible serial interface
- Tested with ESP32-S3 DevKitC-1
- Tested with ESPHome 2026.7.x

## Updating

When using the GitHub external component, ESPHome downloads the component automatically. If a build appears to use an older cached version after an update, use **Clean Build Files** in ESPHome and compile again.

During active development, retaining:

```yaml
refresh: 0s
```

forces ESPHome to check GitHub on every build.

## Credits and licensing

This project builds on:

- iRobot Roomba Open Interface / SCI documentation
- the Arduino Roomba library originally by Mike McCauley and the Davide Cavestro repository used by the earlier implementation
- ESPHome and the ESPHome community

The included `Roomba.h` / `Roomba.cpp` wrapper is derived from the historical Arduino Roomba implementation and retains the relevant GPL v2 licensing attribution in its source headers.

## Disclaimer

This project is not affiliated with or endorsed by iRobot. Use it at your own risk. Incorrect wiring or electrical levels can damage the Roomba or ESP32 hardware.
