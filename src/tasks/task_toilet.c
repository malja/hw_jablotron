#include "task_toilet.h"
#include <stdbool.h>
#include "state_machine.h"
#include "device_valve.h"
#include "device_water_level.h"
// Generováno z Kconfigu
#include "config.h"

#define HALF_FLUSH (uint16_t)((WATER_LEVEL_FULL_THRESHOLD - WATER_LEVEL_EMPTY_THRESHOLD) / 2)

// Deklarováno v main.c
extern bool G_FULL_FLUSH_PRESSED;
extern bool G_HALF_FLUSH_PRESSED;

/// Výška vodní hladiny, než se spustilo splachování
uint16_t G_WATER_LEVEL_BEFORE_FLUSH = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Události, které spouští přechody mezi stavy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Nádobka s vodou je plná
bool event_tank_full() {
    return device_water_level_range(device_water_level_read()) == WATER_LEVEL_FULL;
}

/// Nádobka s vodou je prázdná
bool event_tank_empty() {
    return device_water_level_range(device_water_level_read()) == WATER_LEVEL_EMPTY;
}

/// Bylo stisknuto tlačítko pro plné spláchnutí
bool event_full_flush_button_pressed() {
    return G_FULL_FLUSH_PRESSED;
}

/// Bylo stisknuto tlačítko pro částečné spláchnutí
bool event_half_flush_button_pressed() {
    return G_HALF_FLUSH_PRESSED;
}

/// Buď vytekla již polovina nádobky, nebo je nádobka prázdná
bool event_tank_empty_or_half_flush_done() {
    uint16_t water_level = device_water_level_read();

    bool empty = device_water_level_range(water_level) == WATER_LEVEL_EMPTY;
    // Zkontroluje, jestli už vyteklo dostatečné množství kapaliny
    bool half_flush_done = (G_WATER_LEVEL_BEFORE_FLUSH - water_level) > HALF_FLUSH;

    return empty || half_flush_done;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Přechodové akce
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Uzavře výstup, otevře příchod
void action_filling() {
    device_valve_close(CONFIG_VALVE_OUTPUT_PIN);
    device_valve_open(CONFIG_VALVE_INPUT_PIN);
}

/// Uzavře přívod
void action_tank_full() {
    device_valve_close(CONFIG_VALVE_INPUT_PIN);
}

/// Uzavře přívod, otevře výstup
void action_flush() {
    // Event byl zpracován, může se vyčistit
    G_FULL_FLUSH_PRESSED = false;
    G_HALF_FLUSH_PRESSED = false;

    device_valve_close(CONFIG_VALVE_INPUT_PIN);
    // Hodnota se ukládá vždy, protože funkce je společná pro plné i částečné spláchnutí
    G_WATER_LEVEL_BEFORE_FLUSH = device_water_level_read();
    device_valve_open(CONFIG_VALVE_OUTPUT_PIN);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vytvoření stavového automatu
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static state_machine fsm = {
    .current_state = STATE_FILLING,
    .states = {
        {STATE_FILLING, &event_tank_full, &action_tank_full, STATE_FULL},
        {STATE_FILLING, &event_half_flush_button_pressed, &action_flush, STATE_HALF_FLUSH},
        {STATE_FILLING, &event_full_flush_button_pressed, &action_flush, STATE_FULL_FLUSH},
        {STATE_FULL, &event_full_flush_button_pressed, &action_flush, STATE_FULL_FLUSH},
        {STATE_FULL, &event_half_flush_button_pressed, &action_flush, STATE_HALF_FLUSH},
        {STATE_FULL_FLUSH, &event_tank_empty, &action_filling, STATE_FILLING},
        {STATE_HALF_FLUSH, &event_tank_empty_or_half_flush_done, &action_filling, STATE_FILLING}
    }
}

void task_toilet(void *parameters) {
    while(true) {
        state_machine_run(&fsm);
    }
}