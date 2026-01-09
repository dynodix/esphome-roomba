import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_RX_PIN, CONF_TX_PIN, CONF_UPDATE_INTERVAL

roomba_ns = cg.esphome_ns.namespace("roomba")
RoombaComponent = roomba_ns.class_("RoombaComponent", cg.PollingComponent)

CONF_BRC_PIN = "brc_pin"
CONF_BAUD = "baud"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(RoombaComponent),
        cv.Required(CONF_BRC_PIN): cv.int_,
        cv.Required(CONF_RX_PIN): cv.int_,
        cv.Required(CONF_TX_PIN): cv.int_,
        cv.Optional(CONF_BAUD, default=115200): cv.int_,
        cv.Optional(CONF_UPDATE_INTERVAL, default="30s"): cv.update_interval,
    }
).extend(cv.polling_component_schema("30s"))

async def to_code(config):
    # update_interval in milliseconds for your constructor
    update_ms = int(config[CONF_UPDATE_INTERVAL].total_milliseconds)

    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_BRC_PIN],
        config[CONF_RX_PIN],
        config[CONF_TX_PIN],
        config[CONF_BAUD],
        update_ms,
    )
    await cg.register_component(var, config)
