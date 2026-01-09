import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID

from . import RoombaComponent

CONF_ACTIVITY = "activity"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(RoombaComponent),
        cv.Optional(CONF_ACTIVITY): text_sensor.text_sensor_schema(),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ID])

    if CONF_ACTIVITY in config:
        ts = await text_sensor.new_text_sensor(config[CONF_ACTIVITY])
        cg.add(parent.set_activity_sensor(ts))
