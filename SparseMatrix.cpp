#include "SparseMatrix.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

using namespace std;

// ===========================================================
// Funciones Auxiliares para ver si es numérico o convertirlo
// ===========================================================

static bool isNumber(const string& s) {
    if (s.empty()) return false;

    char* endptr = nullptr;
    strtod(s.c_str(), &endptr);
    return (*endptr == '\0');
}

static double toNumber(const string& s) {
    return stod(s);
}

// =====================================================
// Constructor
// =====================================================

SparseMatrix::SparseMatrix(int r, int c)
    : rows(r), cols(c), rowHead(nullptr), colHead(nullptr) {}

// =====================================================
// Destructor
// =====================================================

SparseMatrix::~SparseMatrix() {

    // Se guarda el puntero rowHead inicial
    RowHeader* rh = rowHead;

    while (rh) {
        Node* curr = rh->first;

        // Se empieza a borrar desde el head hacia adelante hasta que se consuma todas la fila
        while (curr) {
            Node* temp = curr;
            curr = curr->nextInRow;
            delete temp;
        }

        RowHeader* tempH = rh;
        // Se hace el cambio de fila para eliminar toda la hoja, no se necesita usar las cols para llegar a todas las celdas
        rh = rh->nextHeader;
        delete tempH;
    }

    // Elimino las columnas restantes
    ColHeader* ch = colHead;

    while (ch) {
        ColHeader* temp = ch;
        ch = ch->nextHeader;
        delete temp;
    }

    // FIN, toda la matrix eliminada.
}

// =====================================================
// Buscar / crear RowHeader (USADA)
// =====================================================

// El parametro bool existe por seguridad, como un const, ya que a veces no se necesita crear nada, como eliminar un nodo por ejm
RowHeader* SparseMatrix::getRowHeader(int r, bool createIfMissing) {
    RowHeader* prev = nullptr;
    RowHeader* curr = rowHead;

    // recorremos la lista de RowHeaders hasta llegar a la row que buscamos y la guardamos en curr
    while (curr && curr->row < r) {
        prev = curr;
        curr = curr->nextHeader;
    }

    if (curr && curr->row == r)
        return curr;

    if (!createIfMissing)
        return nullptr;

    RowHeader* nuevo = new RowHeader(r);

    // Caso 1: inserta head al inicio de la lista
    // Caso 2: inserta head en medio de heads existentes
    if (!prev) {
        nuevo->nextHeader = rowHead;
        rowHead = nuevo; // apunta a una nueva cabeza
    } else {
        // actualiza punteros
        nuevo->nextHeader = curr;
        prev->nextHeader = nuevo;
    }

    // si la cabeza ya existia la retorné antes pero si no, acá la retorno
    return nuevo;
}

// =====================================================
// Buscar / crear ColHeader (USADA)
// =====================================================

ColHeader* SparseMatrix::getColHeader(int c, bool createIfMissing) {

    // lo mismo que la función de arriba pero para columnas
    ColHeader* prev = nullptr;
    ColHeader* curr = colHead;

    while (curr && curr->col < c) {
        prev = curr;
        curr = curr->nextHeader;
    }

    if (curr && curr->col == c)
        return curr;

    if (!createIfMissing)
        return nullptr;

    ColHeader* nuevo = new ColHeader(c);

    if (!prev) {
        nuevo->nextHeader = colHead;
        colHead = nuevo;
    } else {
        nuevo->nextHeader = curr;
        prev->nextHeader = nuevo;
    }

    return nuevo;
}

// =====================================================
// Find Node
// =====================================================

Node* SparseMatrix::findNode(int r, int c) {
    // si las coordenadas buscadas no existen en la matrix no hay nodo
    if (r < 1 || r > rows || c < 1 || c > cols)
        return nullptr;

    RowHeader* rh = getRowHeader(r, false); // falso porque si existe

    // vemos si existe el RowHeader
    if (!rh)
        return nullptr;

    Node* curr = rh->first;

    // recorremos en las filas 
    while (curr && curr->col < c)
        curr = curr->nextInRow;
    
    // valido que el nodo existe y que su columna sea c
    if (curr && curr->col == c)
        return curr;

    // si valide todo lo anterior y nada cumplio, ok entonces retorno nullptr, no encontro la celda
    return nullptr;
}

// =====================================================
// 1. Insert (USA)
// =====================================================

void SparseMatrix::insert(int r, int c, const string& val) {
    
    // Validación de coordenadas: si está fuera de los límites de la matriz, no hace nada
    if (r < 1 || r > rows || c < 1 || c > cols)
        return;

    // Verifica si ya existe un nodo en esa celda
    Node* existing = findNode(r, c);

    // Si existe, simplemente actualiza su valor y termina, mas flexible que un update solamente
    if (existing) {
        existing->value = val;
        return;
    }

    // Si no existe, obtiene (o crea) los headers de fila y columna
    RowHeader* rh = getRowHeader(r, true);
    ColHeader* ch = getColHeader(c, true);

    // Crea un nuevo nodo con la posición y valor
    Node* nuevo = new Node(r, c, val);

    // Inserción en la lista de la fila (ordenada por columna)
    if (!rh->first || rh->first->col > c) {
        nuevo->nextInRow = rh->first;
        rh->first = nuevo;
    } else {
        Node* curr = rh->first;
        while (curr->nextInRow && curr->nextInRow->col < c)
            curr = curr->nextInRow;
        nuevo->nextInRow = curr->nextInRow;
        curr->nextInRow = nuevo;
    }

    // Inserción en la lista de la columna (ordenada por fila)
    if (!ch->first || ch->first->row > r) {
        nuevo->nextInCol = ch->first;
        ch->first = nuevo;
    } else {
        Node* curr = ch->first;
        while (curr->nextInCol && curr->nextInCol->row < r)
            curr = curr->nextInCol;
        nuevo->nextInCol = curr->nextInCol;
        curr->nextInCol = nuevo;
    }
}


// =====================================================
// Get
// =====================================================

string SparseMatrix::get(int r, int c) {
    // Busca el nodo en la posición (r,c)
    Node* node = findNode(r, c);

    // Si existe, devuelve su valor; si no, devuelve string vacío
    return node ? node->value : "";
}


// =====================================================
// 2. Update
// =====================================================

bool SparseMatrix::update(int r, int c, const string& val) {
    // Busca el nodo en la posición (r,c)
    Node* node = findNode(r, c);

    // Si no existe, no se puede actualizar → false
    if (!node)
        return false;

    // Si existe, cambia su valor y retorna true
    node->value = val;
    return true;
}


// =====================================================
// Remove Cell
// =====================================================

bool SparseMatrix::removeCell(int r, int c) {
    // Busca el nodo a eliminar
    Node* target = findNode(r, c);

    // Si no existe, no hay nada que borrar
    if (!target)
        return false;

    // Obtiene los headers de fila y columna
    RowHeader* rh = getRowHeader(r, false);
    ColHeader* ch = getColHeader(c, false);

    // Quitar de la lista de la fila
    if (rh->first == target) {
        rh->first = target->nextInRow;
    } else {
        Node* curr = rh->first;
        while (curr && curr->nextInRow != target)
            curr = curr->nextInRow;
        if (curr)
            curr->nextInRow = target->nextInRow;
    }

    // Quitar de la lista de la columna
    if (ch->first == target) {
        ch->first = target->nextInCol;
    } else {
        Node* curr = ch->first;
        while (curr && curr->nextInCol != target)
            curr = curr->nextInCol;
        if (curr)
            curr->nextInCol = target->nextInCol;
    }

    // Finalmente libera la memoria del nodo
    delete target;
    return true;
}


// =====================================================
// Remove Row
// =====================================================

void SparseMatrix::removeRow(int r) {
    // Obtiene el header de la fila
    RowHeader* rh = getRowHeader(r, false);

    // Si no existe, no hay nada que borrar
    if (!rh) return;

    // Recorre todos los nodos de la fila y los elimina uno por uno
    Node* curr = rh->first;
    while (curr) {
        Node* next = curr->nextInRow;
        removeCell(curr->row, curr->col);
        curr = next;
    }
}


// =====================================================
// Remove Column
// =====================================================

void SparseMatrix::removeCol(int c) {
    // Obtiene el header de la columna
    ColHeader* ch = getColHeader(c, false);

    // Si no existe, no hay nada que borrar
    if (!ch) return;

    // Recorre todos los nodos de la columna y los elimina uno por uno
    Node* curr = ch->first;
    while (curr) {
        Node* next = curr->nextInCol;
        removeCell(curr->row, curr->col);
        curr = next;
    }
}


// =====================================================
// Remove Range
// =====================================================

void SparseMatrix::removeRange(int r1, int c1, int r2, int c2) {
    // Asegura que r1 <= r2 y c1 <= c2
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);

    // Recorre todas las filas dentro del rango
    for (int r = r1; r <= r2; r++) {
        RowHeader* rh = getRowHeader(r, false);

        // Si la fila no existe, pasa a la siguiente
        if (!rh) continue;

        // Recorre todos los nodos de la fila
        Node* curr = rh->first;
        while (curr) {
            Node* next = curr->nextInRow;

            // Si la columna del nodo está dentro del rango, lo elimina
            if (curr->col >= c1 && curr->col <= c2)
                removeCell(curr->row, curr->col);

            curr = next;
        }
    }
}


// =====================================================
// Sum Row
// =====================================================

double SparseMatrix::sumRow(int r) {
    // Obtiene el header de la fila r
    RowHeader* rh = getRowHeader(r, false);

    // Si no existe la fila, retorna 0
    if (!rh) return 0.0;

    double total = 0.0;
    Node* curr = rh->first;

    // Recorre todos los nodos de la fila
    while (curr) {
        // Si el valor es numérico, lo acumula
        if (isNumber(curr->value))
            total += toNumber(curr->value);

        curr = curr->nextInRow;
    }

    // Devuelve la suma total de la fila
    return total;
}


// =====================================================
// Sum Column
// =====================================================

double SparseMatrix::sumCol(int c) {
    // Obtiene el header de la columna c
    ColHeader* ch = getColHeader(c, false);

    // Si no existe la columna, retorna 0
    if (!ch) return 0.0;

    double total = 0.0;
    Node* curr = ch->first;

    // Recorre todos los nodos de la columna
    while (curr) {
        // Si el valor es numérico, lo acumula
        if (isNumber(curr->value))
            total += toNumber(curr->value);

        curr = curr->nextInCol;
    }

    // Devuelve la suma total de la columna
    return total;
}


// =====================================================
// Sum Range
// =====================================================

double SparseMatrix::sumRange(int r1, int c1, int r2, int c2) {
    // Asegura que r1 <= r2 y c1 <= c2
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);

    double total = 0.0;

    // Recorre todas las filas dentro del rango
    for (int r = r1; r <= r2; r++) {
        RowHeader* rh = getRowHeader(r, false);

        // Si la fila no existe, pasa a la siguiente
        if (!rh) continue;

        Node* curr = rh->first;

        // Recorre todos los nodos de la fila
        while (curr) {
            // Si la columna del nodo está dentro del rango
            if (curr->col >= c1 && curr->col <= c2) {
                // Y el valor es numérico, lo acumula
                if (isNumber(curr->value))
                    total += toNumber(curr->value);
            }
            curr = curr->nextInRow;
        }
    }

    // Devuelve la suma total del rango rectangular
    return total;
}


// =====================================================
// Print Grid (se usó para tests antes de usar la GUI)
// =====================================================

void SparseMatrix::printGrid() {
    cout << "\n===== HOJA DE CALCULO =====\n";

    // Recorre todas las filas y columnas de la matriz
    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            // Obtiene el valor de la celda
            string val = get(r, c);

            // Si está vacío, muestra un "."
            if (val.empty())
                val = ".";

            // Imprime con ancho fijo para que se vea como tabla
            cout << setw(8) << val;
        }
        cout << '\n';
    }

    cout << '\n';
}
