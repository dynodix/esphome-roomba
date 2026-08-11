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
  ESP_LOGD(TAG, "BRC wake pulse on GPIO%u", this->brc_pin_);
  digitalWrite(this->brc_pin_, LOW);
  delay(500);
  digitalWrite(this->brc_pin_, HIGH);
  delay(100);
}

void RoombaComponent::on_command(std::string command) {
  ESP_LOGI(TAG, "Command requested: %s", command.c_str());

  // Preserve the behavior of the original working ESPHomeRoombaComponent:
  // wake the Roomba first, then send the requested OI command.
  this->brc_wakeup_();

  if (command == "turn_on" || command == "turn_off" ||
      command == "start" || command == "stop" || command == "toggle") {
    ESP_LOGI(TAG, "Sending CLEAN toggle (cover)");
    this->roomba_.cover();
  } else if (command == "dock" || command == "return_to_base") {
    ESP_LOGI(TAG, "Sending DOCK");
    this->roomba_.dock();
  } else if (command == "locate") {
    ESP_LOGI(TAG, "Sending LOCATE song");
    uint8_t song[] = {62, 12, 66, 12, 69, 12, 74, 36};
    this->roomba_.safeMode();
    delay(500);
    this->roomba_.song(0, song, sizeof(song));
    this->roomba_.playSong(0);
  } else if (command == "spot" || command == "clean_spot") {
    ESP_LOGI(TAG, "Sending SPOT");
    this->roomba_.spot();
  } else if (command == "wakeup" || command == "brc_wakeup") {
    // The wake pulse above is the requested action.
    ESP_LOGI(TAG, "Roomba wake pulse sent");
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
  ESP_LOGI(TAG, "Roomba setup: BRC=GPIO%u RX=GPIO%u TX=GPIO%u baud=%d",
           this->brc_pin_, this->rx_pin_, this->tx_pin_, this->baud_);

  pinMode(this->brc_pin_, OUTPUT);
  digitalWrite(this->brc_pin_, HIGH);

  // Preserve the original working startup behavior. Roomba::start() from the
  // original library initializes SoftwareSerial at the configured baud itself
  // and sends OI START (128). Do not call serial_.begin() separately and do not
  // force SAFE mode here.
  this->roomba_.start();

  // Roomba TX is relatively weak. EspSoftwareSerial enables an internal pull-up
  // on valid ESP32 RX pins by default; disable it so the Roomba can pull the RX
  // line low reliably.
  this->serial_.enableRxGPIOPullUp(false);

  ESP_LOGI(TAG, "Roomba OI START sent at %d baud; RX pull-up disabled on GPIO%u",
           this->baud_, this->rx_pin_);
}

void RoombaComponent::update() {
  int16_t distance;
  uint16_t voltage;
  int16_t current;
  uint16_t charge;
  uint16_t capacity;
  uint8_t charging;
  int16_t temperature;

  // Flush unsolicited/stale bytes before issuing the sensor-list request,
  // matching the original working component.
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
      Roomba::SensorBatteryTemperature    // 1 byte, C, signed
  };
  uint8_t values[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  ESP_LOGD(TAG, "Requesting Roomba sensor packets with QUERY LIST opcode 149");
  bool success = this->roomba_.getSensorsList(sensors, sizeof(sensors), values, sizeof(values));
  if (!success) {
    ESP_LOGW(TAG, "QUERY LIST 149 timed out; trying single SensorVoltage with opcode 142");

    // Remove any partial/stale bytes before the fallback request.
    while (this->serial_.available()) {
      int raw = this->serial_.read();
      ESP_LOGVV(TAG, "Discarding RX byte after QUERY LIST timeout: 0x%02X", raw & 0xFF);
    }

    delay(50);

    uint8_t voltage_raw[2] = {0, 0};
    bool voltage_ok = this->roomba_.getSensors(Roomba::SensorVoltage, voltage_raw, sizeof(voltage_raw));
    if (!voltage_ok) {
      ESP_LOGW(TAG, "Single SensorVoltage opcode 142 also timed out; RX available=%d", this->serial_.available());
      return;
    }

    uint16_t single_voltage =
        (static_cast<uint16_t>(voltage_raw[0]) << 8) | static_cast<uint16_t>(voltage_raw[1]);

    ESP_LOGI(TAG, "Single SensorVoltage OK: raw=[0x%02X 0x%02X] voltage=%u mV",
             voltage_raw[0], voltage_raw[1], single_voltage);

    if (voltage_sensor_ && voltage_sensor_->state != single_voltage) {
      voltage_sensor_->publish_state(single_voltage);
    }
    return;
  }

  // Decode big-endian OI values. Signed values are explicitly sign-extended;
  // this keeps the corrected decoding from the known working local component.
  auto u16 = [&](int idx) -> uint16_t {
    return (uint16_t(values[idx]) << 8) | uint16_t(values[idx + 1]);
  };
  auto s16 = [&](int idx) -> int16_t {
    return int16_t((uint16_t(values[idx]) << 8) | uint16_t(values[idx + 1]));
  };

  distance = s16(0);
  charging = values[2];
  voltage = u16(3);
  current = s16(5);
  charge = u16(7);
  capacity = u16(9);
  temperature = static_cast<int8_t>(values[11]);

  float battery_level = 0.0f;
  if (capacity > 0) {
    battery_level = 100.0f * (static_cast<float>(charge) / static_cast<float>(capacity));
    if (battery_level < 0.0f) battery_level = 0.0f;
    if (battery_level > 100.0f) battery_level = 100.0f;
  }

  std::string activity = this->get_activity_(charging, current);

  ESP_LOGD(TAG,
           "Sensors OK: distance=%dmm voltage=%umV current=%dmA charge=%umAh capacity=%umAh battery=%.1f%% temp=%dC charging=%u activity=%s",
           distance, voltage, current, charge, capacity, battery_level, temperature, charging, activity.c_str());

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
