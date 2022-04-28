#ifdef __cplusplus
extern "C" {
#endif

// Tento soubor obsahuje implementaci komunikace se senzorem výšky vodní hladiny. Jde o klasický odporový měřák. Čím 
// více vody v nádobce bude, tím menší odpor bude a tím větší hodnotu vyčtu z analogového pinu připojeného na senzor.

typedef enum water_level {
    /// Prázdná nebo přibližně prázdná nádoba
    WATER_LEVEL_EMPTY,
    /// Ani plná, ani prázdná nádoba
    WATER_LEVEL_NEITHER,
    /// Plná nádoba, nebo se k tomu blížící
    WATER_LEVEL_FULL,
} water_level;

/// Provede inicializaci senzoru snímajícího výšku vodní hladiny. Nastaví piny a senzor vypne.
void device_water_level_init();

/// Vyčte aktuální výšku vodní hladiny v rezervoáru. Návratová hodnota odpovídá třem různým stavům, které mohou
/// vzniknout.
///
/// @note Před voláním funkce by měl být senzor inicializován pomocí device_water_level_init.
///
/// @retval WATER_LEVEL_EMPTY Nádobka je prázdná, nebo byla hodnota vyčtená ze senzoru velice blížící se prádné nádrži.
/// @retval WATER_LEVEL_NEITHER Nádobka není ani prázdná, ani plná. 
/// @retval WATER_LEVEL_FULL Nádobka je plná, nebo byla hodnota velice blízko plné nádrži.
uint16_t device_water_level_read();

/// Funkce převede vyčtenou hodnotu na rozsah - plná, prázdná a "něco mezi".
///
/// @note Před voláním funkce by měl být senzor inicializován pomocí device_water_level_init.
///
/// @retval WATER_LEVEL_EMPTY Nádobka je prázdná, nebo byla hodnota vyčtená ze senzoru velice blížící se prádné nádrži.
/// @retval WATER_LEVEL_NEITHER Nádobka není ani prázdná, ani plná. 
/// @retval WATER_LEVEL_FULL Nádobka je plná, nebo byla hodnota velice blízko plné nádrži.
water_level device_water_level_range(uint16_t value);

#ifdef __cplusplus
}
#endif