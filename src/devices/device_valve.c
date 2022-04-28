#include "device_valve.h"
// součást RTOS
#include <RTOS/HAL.h>

void device_valve_input_init(uint8_t pin) {
    // Nastaví vybraný pin na výstupní
    hal_pin_mode(pin, HAL_PIN_MODE_OUT);
    hal_pin_write(pin, HAL_PIN_LOW);
}

void device_valve_input_open(uint8_t pin) {
    // Motorek se  sám po otevření přestane otáčet
    // Zde nebude pin napojený přímo na ventil, protože to by znamenalo, že ho budu napájet z pinu. Místo toho by 
    // vybraný pin nejspíš mířil na relé nebo tranzistor.
    hal_pin_write(pin, HAL_PIN_HIGH);
    // hal_delay(1000);
}

void device_valve_input_close(uint8_t pin) {
    // ventil se po odpojení sám zavře
    hal_pin_write(pin, HAL_PIN_LOW);
}