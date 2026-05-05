| Operación | Complejidad | Justificación |
|---|---|---|
| Insertar celda | O(h + k) | Buscar header O(h), insertar en lista ordenada O(k) |
| Consultar celda | O(h + k) | Buscar header O(h), recorrer fila hasta columna O(k) |
| Modificar celda | O(h + k) | Igual que consultar, actualiza valor in-place |
| Eliminar celda | O(h + k) | Buscar nodo O(h+k), ajustar 2 punteros O(1) |
| Eliminar fila | O(k) | Recorre nodos de la fila, llama removeCell por cada uno |
| Eliminar columna | O(k) | Recorre nodos de la columna, llama removeCell por cada uno |
| Eliminar rango | O(r × k) | r = filas del rango, k = nodos por fila |
| Suma fila/columna | O(k) | Recorre todos los nodos de la fila o columna |
| Suma rango | O(r × k) | Recorre filas del rango |
| PROMEDIO, MAX, MIN | O(r × k) | Igual que suma rango |