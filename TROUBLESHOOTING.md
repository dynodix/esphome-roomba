# Troubleshooting – ESPHome Roomba Component

This document lists common problems and solutions when integrating a Roomba
with ESPHome using this external component.

---

## Roomba Does Not Respond

**Symptoms**
- No sensor updates
- Commands have no effect

**Checks**
- Verify SCI cable pinout with a multimeter
- Ensure common ground between Roomba and ESP32
- Confirm RX/TX are crossed correctly
- Ensure BRC pin is connected and configured

---

## ESP32 Randomly Resets

**Symptoms**
- ESP32 reboots when Wi-Fi connects
- Unstable operation

**Cause**
- Insufficient current from Roomba SCI +5V

**Solutions**
- Add a 470–1000 µF capacitor between 5V and GND
- Add a 0.1 µF ceramic decoupling capacitor
- Shorten power wires
- Power ESP32 via USB instead of Roomba +5V

---

## Sensors Show Incorrect Values

**Symptoms**
- Battery percentage incorrect
- Distance always zero

**Checks**
- Ensure Roomba is awake (BRC pin toggling)
- Confirm correct baud rate (usually 115200)
- Increase update interval (e.g. 60s)

---

## Wi-Fi Works but No ESPHome Entities Appear

**Symptoms**
- ESP32 online but no sensors in Home Assistant

**Checks**
- Verify ESPHome YAML uses the correct `id`
- Confirm `external_components` source is correct
- Check ESPHome logs for component load errors

---

## Build Errors After ESPHome Update

**Symptoms**
- Compilation errors after ESPHome upgrade

**Actions**
- Pull latest version of this repository
- Clean ESPHome build directory
- Review ESPHome release notes for breaking changes

---

## Still Not Working?

Enable verbose logging in ESPHome and inspect serial output:

```yaml
logger:
  level: DEBUG
```

Then review logs for serial or component errors.
