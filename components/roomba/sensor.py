import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

from . import RoombaComponent

CONF_DISTANCE = "distance"
CONF_VOLTAGE = "voltage"
CONF_CURRENT = "current"
CONF_CHARGE = "charge"
CONF_CAPACITY = "capacity"
CONF_BATTERY = "battery"
CONF_TEMPERATURE = "temperature"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(RoombaComponent),
        cv.Optional(CONF_DISTANCE): sensor.sensor_schema(unit_of_measurement="mm", accuracy_decimals=0),
        cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(unit_of_measurement="mV", accuracy_decimals=0),
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(unit_of_measurement="mA", accuracy_decimals=0),
        cv.Optional(CONF_CHARGE): sensor.sensor_schema(unit_of_measurement="mAh", accuracy_decimals=0),
        cv.Optional(CONF_CAPACITY): sensor.sensor_schema(unit_of_measurement="mAh", accuracy_decimals=0),
        cv.Optional(CONF_BATTERY): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=0),
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(unit_of_measurement="°C", accuracy_decimals=0),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ID])

    if CONF_DISTANCE in config:
        s = await sensor.new_sensor(config[CONF_DISTANCE])
        cg.add(parent.set_distance_sensor(s))

    if CONF_VOLTAGE in config:
        s = await sensor.new_sensor(config[CONF_VOLTAGE])
        cg.add(parent.set_voltage_sensor(s))

    if CONF_CURRENT in config:
        s = await sensor.new_sensor(config[CONF_CURRENT])
        cg.add(parent.set_current_sensor(s))

    if CONF_CHARGE in config:
        s = await sensor.new_sensor(config[CONF_CHARGE])
        cg.add(parent.set_charge_sensor(s))

    if CONF_CAPACITY in config:
        s = await sensor.new_sensor(config[CONF_CAPACITY])
        cg.add(parent.set_capacity_sensor(s))

    if CONF_BATTERY in config:
        s = await sensor.new_sensor(config[CONF_BATTERY])
        cg.add(parent.set_battery_percent_sensor(s))

    if CONF_TEMPERATURE in config:
        s = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(parent.set_temperature_sensor(s))
