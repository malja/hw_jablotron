#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Generováno z Kconfigu
#include <config.h>

// Tento soubor obsahuje implementaci jednoduchého stavového automatu.

/// Jednotlivé stavy, které mohou při běhu zařízení nastat.
typedef enum state {
    /// Probíhá plnění zásobníku. Zároveň výchozí stav po spuštění.
    STATE_FILLING,
    /// Plný zásobník s vodou.
    STATE_FULL,
    /// Vyžádáno/probíhající kompletní spláchnutí.
    STATE_FULL_FLUSH,
    /// Vyžádáno/probíhající částečné spláchnutí.
    STATE_HALF_FLUSH
} state;

typedef bool (state_event_fn)(void);
typedef void (state_action_fn)(void);

typedef struct state_record {
    state state;
    state_event_fn *event;
    state_action_fn *action;
    state next_state;
} state_record;

typedef struct state_machine {
    state current_state;
    state_record states[CONFIG_STATE_MACHINE_STATES_COUNT];
} state_machine;

/// Tato funkce by měla být spuštěna v hlavní smyčce projektu, aby zpracovala jednotlivé stavy a přechody mezi nimi.
///
/// @param self[in] Struktura se stavovým automatem. Nesmí být NULL.
void state_machine_run(state_machine *self);

#ifdef __cplusplus
}
#endif