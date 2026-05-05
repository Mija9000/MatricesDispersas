# Justificación de la Estructura de Datos y Análisis de Complejidad

## Justificación de la Estructura de Datos

Se utilizó una **matriz dispersa implementada con listas enlazadas cruzadas** porque:

1. **Eficiencia de memoria**: Las hojas de cálculo son inherentemente dispersas (pocas celdas ocupadas). Una matriz densa gastaría memoria en celdas vacías.
2. **Operaciones rápidas**: Buscar, insertar y eliminar celdas es O(h + k) donde h es el número de filas ocupadas y k el número de columnas ocupadas, mucho menor que O(rows × cols) en matriz densa.
3. **Escalabilidad**: Permite trabajar con hojas de cualquier tamaño sin consumir memoria innecesaria.
4. **Operaciones estructurales**: Eliminar filas/columnas es eficiente al solo recorrer nodos existentes, no la matriz completa.

La estructura mantiene dos listas de headers (filas y columnas) conectadas a nodos de datos, permitiendo acceso rápido desde ambas dimensiones.

---

## Análisis de Complejidad

| Operación | Complejidad | Justificación |
|---|---|---|
| Insertar celda | O(h + k) | Buscar header O(h), insertar en lista ordenada O(k) |
| Consultar celda | O(h + k) | Buscar header O(h), recorrer hasta columna O(k) |
| Modificar celda | O(h + k) | Igual que consultar, actualiza valor in-place |
| Eliminar celda | O(h + k) | Buscar nodo O(h+k), desenlazar punteros O(1) |
| Eliminar fila | O(k·c) | k nodos en fila, cada eliminar actualiza columnas O(c) |
| Eliminar columna | O(k·r) | k nodos en columna, cada eliminar actualiza filas O(r) |
| Eliminar rango | O(r·k·c) | r filas, k nodos/fila, c columnas actualizadas |
| Suma fila/columna | O(k) | Recorre solo nodos ocupados |
| Suma rango | O(r·k) | Recorre filas del rango |
| PROMEDIO/MAX/MIN | O(r·k) | Igual que suma rango |

**Donde:** h = filas ocupadas, k = celdas ocupadas por fila, r = filas del rango, c = columnas afectadas.