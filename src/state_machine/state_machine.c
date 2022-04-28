#include <assert.h>
#include "state_machine.h"

void state_machine_run(state_machine *self) {
    assert(self != NULL);

    // Projde celou tabulku
    for (uint8_t index = 0; index < CONFIG_STATE_MACHINE_STATES_COUNT; index++) {
        // Zajímá nás jen záznam s aktuálním stavem
        if (self->current_state == self->states[index].state) {
            // Otestuje, jestli se má přejít do dalšího stavu
            if (self->states[index].event()) {
                // Spustí se přechodové funkce
                self->states[index].action();
                // Přejde  se na další stav
                self->current_state = self->states[index].next_state;
                // Pro tento průchod je konec
                break;
            }
        }
    }
}