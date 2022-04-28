#include "device_valve.h"
#include "device_water_level.h"
// Generován z KConfigu
#include "config.h"
// Součást RTOS
#include <RTOS/HAL.h>
#include "tasks.h"

/// Stav stisku tlačítka plného spláchnutí. Hodnota je nastavena z interruptu navázaného na tlačítko.
volatile bool G_FULL_FLUSH_PRESSED = false;
/// Stav stisku tlačítka částečného spláchnutí. Hodnota je nastavena z interruptu navázaného na tlačítko.
volatile bool G_HALF_FLUSH_PRESSED = false;

/// Zpracování stisku tlačítka kompletního spláchnutí.
void interrupt_full_flush() {
    G_FULL_FLUSH_PRESSED = true;
}

/// Zpracování stisku tlačítka kompletního spláchnutí.
void interrupt_half_flush() {
    G_FULL_FLUSH_PRESSED = true;
}

void setup() {
    // Nastavení ventilů
    device_valve_init(CONFIG_VALVE_INPUT_PIN);
    device_valve_init(CONFIG_VALVE_OUTPUT_PIN);
    // Nastavení výšky vodní hladiny
    device_water_level_init();

    // Zapne interrupty na vstupních tlačítkách
    hal_interrupt_attach(CONFIG_BUTTON_FULL_FLUSH_PIN, HAL_INT_FALLING, &interrupt_full_flush);
    hal_interrupt_attach(CONFIG_BUTTON_HALF_FLUSH_PIN, HAL_INT_FALLING, &interrupt_half_flush);
}

int main(void) {
    setup();

    // Handle vytvořeného tasku
    hal_task *task = NULL;
    // Parametry: funkce, název tasku, velikost stacku, parametr, priorita, ukazatel na handle
    hal_return ret = hal_task_start(&task_toilet, "Toilet", CONFIG_TASK_TOILET_STACK, NULL, HAL_TASK_PRIORITY_NORMAL, &task);
    if (ret != HAL_RETURN_OK) {
        HAL_LOG_ERR("Could not start toilet task %d", ret);
        // dead end
        hal_halt();
    }
}