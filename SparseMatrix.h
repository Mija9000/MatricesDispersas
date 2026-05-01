#include <iostream>
#pragma once
#include "Nodes.h"
#include <string>
using namespace std;
class SparseMatrix {
private:
    int rows, cols;

    // Inicio de listas de headers
    RowHeader* rowHead;
    ColHeader* colHead;

    // ==========================
    // Helpers internos
    // ==========================
    RowHeader* getRowHeader(int r, bool createIfMissing);
    ColHeader* getColHeader(int c, bool createIfMissing);

    // Es privado, ya que el usuario no lo usa como tal
    Node* findNode(int r, int c);

public:
    SparseMatrix(int r, int c);
    ~SparseMatrix();

    // ==========================
    // Operaciones básicas
    // ==========================
    void insert(int r, int c, const string& val);
    string get(int r, int c);

    bool update(int r, int c, const string& val);
    bool removeCell(int r, int c);

    // ==========================
    // Operaciones estructurales
    // ==========================
    void removeRow(int r);
    void removeCol(int c);
    void removeRange(int r1, int c1, int r2, int c2);

    // ==========================
    // Agregaciones
    // ==========================
    double sumRow(int r);
    double sumCol(int c);
    double sumRange(int r1, int c1, int r2, int c2);

    // ==========================
    // Utilidad
    // ==========================
    void printGrid();
};