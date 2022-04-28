#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Tento soubor obsahuje implementaci komunikace s vstupním a výstupním ventilem. Přestože teoreticky ovládání každého
// ventilu může být rozdílné, vybral jsem si nejjednodušší případ, kdy je oba ventily možné ovládat podobně.
// Jak ventil na vstupu, tak na výstupu je bez napětí uzavřen. Po přivedení napětí se ventil otevře. Tím je zajištěno,
// že v případě výpadku nedojde k protékání.

/// Provede inicializaci ventilu - nastaví ovládací pin na výstupní ventil uzavře.
///
/// @param pin[in] Výběr kontrolního PINu.
void device_valve_init(uint8_t pin);

/// Otevře ventil.
///
/// @note Před voláním této funkce by měl být ventil inicializován pomocí device_valve_init.
///
/// @param pin[in] Výběr kontrolního PINu.
void device_valve_open(uint8_t pin);

/// Uzavře ventil.
///
/// @note Před voláním této funkce by měl být ventil inicializován pomocí device_valve_init.
///
/// @param pin[in] Výběr kontrolního PINu.
void device_valve_input_close(uint8_t pin);

#ifdef __cplusplus
}
#endif