#include "device_water_level.h"
#include <stdint.h>
#include <config.h>
#include <RTOS/HAL.h>

void device_water_level_init() {
    // Tento pin ovládá napájení senzoru
    hal_pin_mode(CONFIG_WATER_LEVEL_POWER_PIN, HAL_PIN_MODE_OUT);
    // tento pin umožňuje číst ze senzoru
    hal_pin_mode(CONFIG_WATER_LEVEL_READ_PIN, HAL_PIN_MODE_IN);

    // Vypne senzor
    hal_pin_write(CONFIG_WATER_LEVEL_POWER_PIN, HAL_PIN_LOW);
}

uint16_t device_water_level_read() {
    uint16_t level = 0;
    
    // Senzor opět nebude k jednotce připojen napřímo, ale pravděpodobně přes nějaké relé/tranzistor kvůli napájení.
    hal_pin_write(CONFIG_WATER_LEVEL_POWER_PIN, HAL_PIN_HIGH);
	hal_delay_ms(10);
	level = hal_pin_read(CONFIG_WATER_LEVEL_READ_PIN)
	hal_pin_write(CONFIG_WATER_LEVEL_POWER_PIN, HAL_PIN_LOW);

    return level;
}

water_level device_water_level_range(uint16_t value) {
    if (value <= CONFIG_WATER_LEVEL_EMPTY_THRESHOLD) {
        return WATER_LEVEL_EMPTY;
    } else if (value >= CONFIG_WATER_LEVEL_FULL_THRESHOLD) {
        return WATER_LEVEL_FULL;
    } else {
        return WATER_LEVEL_NEITHER;
    }
}