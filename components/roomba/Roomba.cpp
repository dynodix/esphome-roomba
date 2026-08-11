// Minimal Roomba Open Interface wrapper used by the ESPHome component.
// Based on the Arduino Roomba library by Mike McCauley (GPL v2 option).

#include "Roomba.h"

Roomba::Roomba(SoftwareSerial *serial, Baud baud)
    : serial_(serial), baud_(this->baudCodeToBaudRate_(baud)) {}

uint32_t Roomba::baudCodeToBaudRate_(Baud baud) const {
  switch (baud) {
    case Baud300: return 300;
    case Baud600: return 600;
    case Baud1200: return 1200;
    case Baud2400: return 2400;
    case Baud4800: return 4800;
    case Baud9600: return 9600;
    case Baud14400: return 14400;
    case Baud19200: return 19200;
    case Baud28800: return 28800;
    case Baud38400: return 38400;
    case Baud57600: return 57600;
    case Baud115200: return 115200;
    default: return 115200;
  }
}

void Roomba::start() {
  this->serial_->begin(this->baud_);
  this->serial_->write(128);
}

void Roomba::safeMode() {
  this->serial_->write(131);
}

void Roomba::dock() {
  this->serial_->write(143);
}

void Roomba::cover() {
  this->serial_->write(135);
}

void Roomba::spot() {
  this->serial_->write(134);
}

void Roomba::song(uint8_t song_number, const uint8_t *data, int len) {
  this->serial_->write(140);
  this->serial_->write(song_number);
  this->serial_->write(static_cast<uint8_t>(len >> 1));
  this->serial_->write(data, len);
}

void Roomba::playSong(uint8_t song_number) {
  this->serial_->write(141);
  this->serial_->write(song_number);
}

bool Roomba::getData_(uint8_t *dest, uint8_t len) {
  while (len-- > 0) {
    unsigned long start_time = millis();
    while (!this->serial_->available()) {
      if (millis() > start_time + ROOMBA_READ_TIMEOUT)
        return false;
    }
    *dest++ = static_cast<uint8_t>(this->serial_->read());
  }
  return true;
}

bool Roomba::getSensorsList(uint8_t *packet_ids, uint8_t num_packet_ids, uint8_t *dest, uint8_t len) {
  this->serial_->write(149);
  this->serial_->write(num_packet_ids);
  this->serial_->write(packet_ids, num_packet_ids);
  return this->getData_(dest, len);
}
