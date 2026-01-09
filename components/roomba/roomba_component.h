#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include <string>
#include <Roomba.h>
#include <SoftwareSerial.h>

namespace esphome {
namespace roomba {

class RoombaComponent : public PollingComponent {
 public:
  RoombaComponent(uint8_t brc_pin, uint8_t rx_pin, uint8_t tx_pin, int baud, uint32_t update_interval_ms);

  // Setters from codegen (sensor.py / text_sensor.py)
  void set_distance_sensor(sensor::Sensor *s) { distance_sensor_ = s; }
  void set_voltage_sensor(sensor::Sensor *s) { voltage_sensor_ = s; }
  void set_current_sensor(sensor::Sensor *s) { current_sensor_ = s; }
  void set_charge_sensor(sensor::Sensor *s) { charge_sensor_ = s; }
  void set_capacity_sensor(sensor::Sensor *s) { capacity_sensor_ = s; }
  void set_battery_percent_sensor(sensor::Sensor *s) { battery_percent_sensor_ = s; }
  void set_temperature_sensor(sensor::Sensor *s) { temperature_sensor_ = s; }
  void set_activity_sensor(text_sensor::TextSensor *s) { activity_sensor_ = s; }

  void setup() override;
  void update() override;

  void on_command(std::string command);

 protected:
  Roomba::Baud map_baud_(int baud);

  void brc_wakeup_();
  std::string get_activity_(uint8_t charging, int16_t current);

  uint8_t brc_pin_;
  uint8_t rx_pin_;
  uint8_t tx_pin_;
  int baud_;

  // Keep pointers so we can flush serial like your original code
  SoftwareSerial serial_;
  Roomba roomba_;

  sensor::Sensor *distance_sensor_{nullptr};
  sensor::Sensor *voltage_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *charge_sensor_{nullptr};
  sensor::Sensor *capacity_sensor_{nullptr};
  sensor::Sensor *battery_percent_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  text_sensor::TextSensor *activity_sensor_{nullptr};
};

}  // namespace roomba
}  // namespace esphome
