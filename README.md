ESPHome Roomba External Component

This repository provides an ESPHome external component for controlling and monitoring legacy iRobot Roomba vacuum cleaners via their serial (SCI) interface.

It is designed to replace older, now-removed custom_component-based configurations and is fully compatible with ESPHome 2025.x and later.

The component has been tested on ESP32-S3 hardware and integrates cleanly with Home Assistant through ESPHome.

Features

Control Roomba actions:

Start / Stop cleaning

Dock (return to base)

Spot cleaning

Locate (play sound)

Wake-up via BRC pin

Read and expose sensors:

Distance traveled

Battery voltage

Battery current

Battery charge

Battery capacity

Battery percentage

Battery temperature

Activity state (Cleaning / Docked / Charging / Lost)

Native ESPHome entities:

Sensors

Text sensor

Buttons

Home Assistant services (defined in YAML, not C++)

Compatible with Home Assistant Green (no SSH required)

Supported Hardware

Roomba models with SCI / serial interface
(e.g. Roomba 5xx series such as 560)

ESP32 (ESP32-S3 confirmed)

Connection via:

Roomba BRC (wake) pin

SoftwareSerial (RX/TX)

⚠️ This component relies on the Roomba Open Interface and requires proper electrical interfacing. Ensure voltage levels and wiring are correct for your model.

Repository Structure
components/
  roomba/
    __init__.py              # ESPHome component definition
    roomba_component.h       # C++ component header
    roomba_component.cpp     # C++ component implementation
    sensor.py                # Sensor platform bindings
    text_sensor.py           # Text sensor bindings

esp-roomba.yaml              # Example ESPHome configuration

Installation (ESPHome External Component)
1. Add the component to your ESPHome configuration
external_components:
  - source:
      type: github
      repo: dynodix/esphome-roomba
      ref: main


ESPHome automatically loads components from the components/ directory.

2. Example ESPHome configuration
roomba:
  id: roomba1
  brc_pin: 9
  rx_pin: 8
  tx_pin: 7
  baud: 115200
  update_interval: 30s

3. Sensors
sensor:
  - platform: roomba
    id: roomba1
    distance:
      name: "Roomba Distance"
    voltage:
      name: "Roomba Voltage"
    current:
      name: "Roomba Current"
    charge:
      name: "Roomba Charge"
    capacity:
      name: "Roomba Capacity"
    battery:
      name: "Roomba Battery"
    temperature:
      name: "Roomba Temperature"

4. Activity text sensor
text_sensor:
  - platform: roomba
    id: roomba1
    activity:
      name: "Roomba Activity"

5. Control buttons
button:
  - platform: template
    name: "Roomba Start"
    on_press:
      - lambda: |-
          id(roomba1).on_command("start");

  - platform: template
    name: "Roomba Dock"
    on_press:
      - lambda: |-
          id(roomba1).on_command("dock");

  - platform: template
    name: "Roomba Locate"
    on_press:
      - lambda: |-
          id(roomba1).on_command("locate");

6. Home Assistant services (optional)

Because ESPHome 2025.x has limitations around string arguments in C++ services, services are defined in YAML:

api:
  services:
    - service: roomba_start
      then:
        - lambda: |-
            id(roomba1).on_command("start");

    - service: roomba_dock
      then:
        - lambda: |-
            id(roomba1).on_command("dock");

    - service: roomba_locate
      then:
        - lambda: |-
            id(roomba1).on_command("locate");


These services will appear in Home Assistant as standard ESPHome services.

Technical Notes

This component uses PollingComponent with a configurable update interval.

Roomba baud rates are mapped explicitly to the Roomba library enum.

No deprecated ESPHome features (custom_component, register_service) are used.

Fully compatible with:

ESPHome 2025.x

Home Assistant Green

Designed to be future-proof against ESPHome API changes.

Credits

Based on the Roomba Open Interface

Uses the davidecavestro/Roomba
 Arduino library

Inspired by earlier ESPHome Roomba integrations, rewritten as a modern external component

Disclaimer

This project is not affiliated with iRobot.
Use at your own risk. Incorrect wiring or commands may damage hardware.
