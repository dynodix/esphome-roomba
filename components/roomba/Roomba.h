// Minimal Roomba Open Interface wrapper used by the ESPHome component.
//
// Based on the Arduino Roomba library by Mike McCauley (Copyright 2010),
// used under the GPL v2 open-source licensing option. This reduced wrapper
// keeps only the API required by this ESPHome component.

#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

#define ROOMBA_READ_TIMEOUT 200

class Roomba {
 public:
  enum Baud {
    Baud300 = 0,
    Baud600 = 1,
    Baud1200 = 2,
    Baud2400 = 3,
    Baud4800 = 4,
    Baud9600 = 5,
    Baud14400 = 6,
    Baud19200 = 7,
    Baud28800 = 8,
    Baud38400 = 9,
    Baud57600 = 10,
    Baud115200 = 11,
  };

  enum ChargeState {
    ChargeStateNotCharging = 0,
    ChargeStateReconditioningCharging = 1,
    ChargeStateFullChanrging = 2,
    ChargeStateTrickleCharging = 3,
    ChargeStateWaiting = 4,
    ChargeStateFault = 5,
  };

  enum Sensor {
    SensorDistance = 19,
    SensorChargingState = 21,
    SensorVoltage = 22,
    SensorCurrent = 23,
    SensorBatteryTemperature = 24,
    SensorBatteryCharge = 25,
    SensorBatteryCapacity = 26,
  };

  Roomba(SoftwareSerial *serial, Baud baud);

  void start();
  void safeMode();
  void dock();
  void cover();
  void spot();
  void song(uint8_t song_number, const uint8_t *data, int len);
  void playSong(uint8_t song_number);

  bool getSensorsList(uint8_t *packet_ids, uint8_t num_packet_ids, uint8_t *dest, uint8_t len);

 protected:
  uint32_t baudCodeToBaudRate_(Baud baud) const;
  bool getData_(uint8_t *dest, uint8_t len);

  SoftwareSerial *serial_;
  uint32_t baud_;
};
