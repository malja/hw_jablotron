#ifdef __cplusplus
extern "C" {
#endif

/// Tato funkce reprezentuje separátní task, který běží na zařízení a má za úkol ovládání záchodu. 
///
/// @param parameters[in] Nepovinné parametry. Hodnota není využita.
void task_toilet(void *parameters);

#ifdef __cplusplus
}
#endif