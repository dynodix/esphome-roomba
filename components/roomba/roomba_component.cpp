#include "roomba_component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace roomba {

static const char *const TAG = "roomba";

Roomba::Baud RoombaComponent::map_baud_(int baud) {
  switch (baud) {
    case 115200:
      return Roomba::Baud115200;
    case 57600:
      return Roomba::Baud57600;
    case 19200:
      return Roomba::Baud19200;
    case 9600:
      return Roomba::Baud9600;
    default:
      ESP_LOGW(TAG, "Unsupported baud %d, defaulting to 115200", baud);
      return Roomba::Baud115200;
  }
}


RoombaComponent::RoombaComponent(uint8_t brc_pin, uint8_t rx_pin, uint8_t tx_pin, int baud, uint32_t update_interval_ms)
    : PollingComponent(update_interval_ms),
      brc_pin_(brc_pin),
      rx_pin_(rx_pin),
      tx_pin_(tx_pin),
      baud_(baud),
      serial_(rx_pin, tx_pin),
      roomba_(&serial_, this->map_baud_(baud)) {
}


void RoombaComponent::brc_wakeup_() {
  digitalWrite(this->brc_pin_, LOW);
  delay(500);
  digitalWrite(this->brc_pin_, HIGH);
  delay(100);
}

void RoombaComponent::on_command(std::string command) {
  this->brc_wakeup_();

  if (command == "turn_on" || command == "turn_off" || command == "start" || command == "stop") {
    this->roomba_.cover();
  } else if (command == "dock" || command == "return_to_base") {
    this->roomba_.dock();
  } else if (command == "locate") {
    uint8_t song[] = {62, 12, 66, 12, 69, 12, 74, 36};
    this->roomba_.safeMode();
    delay(500);
    this->roomba_.song(0, song, sizeof(song));
    this->roomba_.playSong(0);
  } else if (command == "spot" || command == "clean_spot") {
    this->roomba_.spot();
  } else if (command == "wakeup" || command == "brc_wakeup") {
    this->brc_wakeup_();
  } else {
    ESP_LOGW(TAG, "Unknown command: %s", command.c_str());
  }
}

std::string RoombaComponent::get_activity_(uint8_t charging, int16_t current) {
  bool charging_state =
      charging == Roomba::ChargeStateReconditioningCharging ||
      charging == Roomba::ChargeStateFullChanrging ||
      charging == Roomba::ChargeStateTrickleCharging;

  if (charging_state) return "Charging";
  if (current > -50) return "Docked";
  if (current < -300) return "Cleaning";
  return "Lost";
}

void RoombaComponent::setup() {
  pinMode(this->brc_pin_, OUTPUT);
  digitalWrite(this->brc_pin_, HIGH);

  // Start serial + Roomba
  this->serial_.begin(this->baud_);
  this->roomba_.start();

  // Expose ESPHome native API service (kept from your original approach)
  // register_service docs: :contentReference[oaicite:1]{index=1}
  // this->register_service(&RoombaComponent::on_command, "command", {"command"});
}

void RoombaComponent::update() {
  int16_t distance;
  uint16_t voltage;
  int16_t current;
  uint16_t charge;
  uint16_t capacity;
  uint8_t charging;
  int16_t temperature;

  // Flush serial buffers
  while (this->serial_.available()) {
    this->serial_.read();
  }

  uint8_t sensors[] = {
      Roomba::SensorDistance,             // 2 bytes, mm, signed
      Roomba::SensorChargingState,        // 1 byte
      Roomba::SensorVoltage,              // 2 bytes, mV, unsigned
      Roomba::SensorCurrent,              // 2 bytes, mA, signed
      Roomba::SensorBatteryCharge,        // 2 bytes, mAh, unsigned
      Roomba::SensorBatteryCapacity,      // 2 bytes, mAh, unsigned
      Roomba::SensorBatteryTemperature    // 1 byte, °C, signed
  };
  uint8_t values[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  bool success = this->roomba_.getSensorsList(sensors, sizeof(sensors), values, sizeof(values));
  if (!success) return;

  distance = values[0] * 256 + values[1];
  charging = values[2];
  voltage  = values[3] * 256 + values[4];
  current  = values[5] * 256 + values[6];
  charge   = values[7] * 256 + values[8];
  capacity = values[9] * 256 + values[10];
  temperature = static_cast<int8_t>(values[11]);  // signed 1 byte

  float battery_level = (capacity > 0) ? (100.0f * (static_cast<float>(charge) / static_cast<float>(capacity))) : 0.0f;
  std::string activity = this->get_activity_(charging, current);

  if (distance_sensor_ && distance_sensor_->state != distance) distance_sensor_->publish_state(distance);
  if (voltage_sensor_ && voltage_sensor_->state != voltage) voltage_sensor_->publish_state(voltage);
  if (current_sensor_ && current_sensor_->state != current) current_sensor_->publish_state(current);
  if (charge_sensor_ && charge_sensor_->state != charge) charge_sensor_->publish_state(charge);
  if (capacity_sensor_ && capacity_sensor_->state != capacity) capacity_sensor_->publish_state(capacity);
  if (battery_percent_sensor_ && battery_percent_sensor_->state != battery_level) battery_percent_sensor_->publish_state(battery_level);
  if (temperature_sensor_ && temperature_sensor_->state != temperature) temperature_sensor_->publish_state(temperature);

  if (activity_sensor_) activity_sensor_->publish_state(activity);
}

}  // namespace roomba
}  // namespace esphome
