#include <iostream>
#pragma once
#include <string>
using namespace std;
// ============================
// Nodo de datos (celda)
// ============================
struct Node {
    int row, col;
    string value;

    Node* nextInRow;
    Node* nextInCol;

    Node(int r, int c, const string& val)
        : row(r), col(c), value(val),
          nextInRow(nullptr), nextInCol(nullptr) {}
};

// ============================
// Header de fila
// ============================
struct RowHeader {
    int row;
    RowHeader* nextHeader;
    Node* first;

    RowHeader(int r)
        : row(r), nextHeader(nullptr), first(nullptr) {}
};

// ============================
// Header de columna
// ============================
struct ColHeader {
    int col;
    ColHeader* nextHeader;
    Node* first;

    ColHeader(int c)
        : col(c), nextHeader(nullptr), first(nullptr) {}
};