#include "SparseMatrix.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

using namespace std;

// =====================================================
// Helpers numéricos
// =====================================================

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
    RowHeader* rh = rowHead;

    while (rh) {
        Node* curr = rh->first;

        while (curr) {
            Node* temp = curr;
            curr = curr->nextInRow;
            delete temp;
        }

        RowHeader* tempH = rh;
        rh = rh->nextHeader;
        delete tempH;
    }

    ColHeader* ch = colHead;

    while (ch) {
        ColHeader* temp = ch;
        ch = ch->nextHeader;
        delete temp;
    }
}

// =====================================================
// Buscar / crear RowHeader
// =====================================================

RowHeader* SparseMatrix::getRowHeader(int r, bool createIfMissing) {
    RowHeader* prev = nullptr;
    RowHeader* curr = rowHead;

    while (curr && curr->row < r) {
        prev = curr;
        curr = curr->nextHeader;
    }

    if (curr && curr->row == r)
        return curr;

    if (!createIfMissing)
        return nullptr;

    RowHeader* nuevo = new RowHeader(r);

    if (!prev) {
        nuevo->nextHeader = rowHead;
        rowHead = nuevo;
    } else {
        nuevo->nextHeader = curr;
        prev->nextHeader = nuevo;
    }

    return nuevo;
}

// =====================================================
// Buscar / crear ColHeader
// =====================================================

ColHeader* SparseMatrix::getColHeader(int c, bool createIfMissing) {
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
    if (r < 1 || r > rows || c < 1 || c > cols)
        return nullptr;

    RowHeader* rh = getRowHeader(r, false);

    if (!rh)
        return nullptr;

    Node* curr = rh->first;

    while (curr && curr->col < c)
        curr = curr->nextInRow;

    if (curr && curr->col == c)
        return curr;

    return nullptr;
}

// =====================================================
// Insert
// =====================================================

void SparseMatrix::insert(int r, int c, const string& val) {
    
    // Capa 2
    if (r < 1 || r > rows || c < 1 || c > cols)
        return;

    // Capa 3
    Node* existing = findNode(r, c);

    if (existing) {
        existing->value = val;
        return;
    }

    // Capa 4, crea header fila 1
    RowHeader* rh = getRowHeader(r, true);
    ColHeader* ch = getColHeader(c, true);

    Node* nuevo = new Node(r, c, val);

    // Insertar en fila
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

    // Insertar en columna
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
    Node* node = findNode(r, c);
    return node ? node->value : "";
}

// =====================================================
// Update
// =====================================================

bool SparseMatrix::update(int r, int c, const string& val) {
    Node* node = findNode(r, c);

    if (!node)
        return false;

    node->value = val;
    return true;
}

// =====================================================
// Remove Cell
// =====================================================

bool SparseMatrix::removeCell(int r, int c) {
    Node* target = findNode(r, c);

    if (!target)
        return false;

    RowHeader* rh = getRowHeader(r, false);
    ColHeader* ch = getColHeader(c, false);

    // Quitar de fila
    if (rh->first == target) {
        rh->first = target->nextInRow;
    } else {
        Node* curr = rh->first;

        while (curr && curr->nextInRow != target)
            curr = curr->nextInRow;

        if (curr)
            curr->nextInRow = target->nextInRow;
    }

    // Quitar de columna
    if (ch->first == target) {
        ch->first = target->nextInCol;
    } else {
        Node* curr = ch->first;

        while (curr && curr->nextInCol != target)
            curr = curr->nextInCol;

        if (curr)
            curr->nextInCol = target->nextInCol;
    }

    delete target;
    return true;
}

// =====================================================
// Remove Row
// =====================================================

void SparseMatrix::removeRow(int r) {
    RowHeader* rh = getRowHeader(r, false);

    if (!rh) return;

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
    ColHeader* ch = getColHeader(c, false);

    if (!ch) return;

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
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);

    for (int r = r1; r <= r2; r++) {
        RowHeader* rh = getRowHeader(r, false);

        if (!rh) continue;

        Node* curr = rh->first;

        while (curr) {
            Node* next = curr->nextInRow;

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
    RowHeader* rh = getRowHeader(r, false);

    if (!rh) return 0.0;

    double total = 0.0;
    Node* curr = rh->first;

    while (curr) {
        if (isNumber(curr->value))
            total += toNumber(curr->value);

        curr = curr->nextInRow;
    }

    return total;
}

// =====================================================
// Sum Column
// =====================================================

double SparseMatrix::sumCol(int c) {
    ColHeader* ch = getColHeader(c, false);

    if (!ch) return 0.0;

    double total = 0.0;
    Node* curr = ch->first;

    while (curr) {
        if (isNumber(curr->value))
            total += toNumber(curr->value);

        curr = curr->nextInCol;
    }

    return total;
}

// =====================================================
// Sum Range
// =====================================================

double SparseMatrix::sumRange(int r1, int c1, int r2, int c2) {
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);

    double total = 0.0;

    for (int r = r1; r <= r2; r++) {
        RowHeader* rh = getRowHeader(r, false);

        if (!rh) continue;

        Node* curr = rh->first;

        while (curr) {
            if (curr->col >= c1 && curr->col <= c2) {
                if (isNumber(curr->value))
                    total += toNumber(curr->value);
            }

            curr = curr->nextInRow;
        }
    }

    return total;
}

// =====================================================
// Print Grid
// =====================================================

void SparseMatrix::printGrid() {
    cout << "\n===== HOJA DE CALCULO =====\n";

    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            string val = get(r, c);

            if (val.empty())
                val = ".";

            cout << setw(8) << val;
        }
        cout << '\n';
    }

    cout << '\n';
}