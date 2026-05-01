#include <iostream>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "Spreadsheet.h"

using namespace std;

// ============================
// Helpers
// ============================

// Detecta si es número
bool isNumber(const string& s) {
    if (s.empty()) return false;
    char* endptr = nullptr;
    strtod(s.c_str(), &endptr);
    return (*endptr == '\0');
}

// Detecta si es referencia tipo A1
bool isCellRef(const string& s) {
    return !s.empty() && isalpha(s[0]);
}

// Convierte "A1" → (fila, col)
pair<int,int> parseCell(const string& ref) {
    int col = toupper(ref[0]) - 'A' + 1;
    int row = stoi(ref.substr(1));
    return {row, col};
}

// Detecta =SUMA(...)
bool isSUMA(const string& s) {
    return s.rfind("=SUMA(", 0) == 0;
}

// Parsear rango A1:C3
pair<pair<int,int>, pair<int,int>> parseRange(const string& inside) {
    size_t colon = inside.find(':');

    string left = inside.substr(0, colon);
    string right = inside.substr(colon + 1);

    auto [r1,c1] = parseCell(left);
    auto [r2,c2] = parseCell(right);

    return {{r1,c1}, {r2,c2}};
}

// Formato bonito
string formatDouble(double val) {
    if (val == (int)val)
        return to_string((int)val);

    ostringstream oss;
    oss << fixed << setprecision(2) << val;
    return oss.str();
}

bool isPROMEDIO(const string& s) {
    return s.rfind("=PROMEDIO(", 0) == 0;
}
bool isMAX(const string& s) {
    return s.rfind("=MAX(", 0) == 0;
}

bool isMIN(const string& s) {
    return s.rfind("=MIN(", 0) == 0;
}
// ============================
// CLASE
// ============================

Spreadsheet::Spreadsheet(int rows,int cols)
    : matrix(rows,cols) {}

SparseMatrix& Spreadsheet::getMatrix() {
    return matrix;
}
// Resolver token
double Spreadsheet::resolveToken(const string& token) {
    if (isCellRef(token)) {
        auto [r,c] = parseCell(token);
        string val = matrix.get(r,c);

        if (!isNumber(val))
            return 0;

        return stod(val);
    }

    if (isNumber(token))
        return stod(token);

    return 0;
}
// ============================
// EXPRESIONES NORMALES
// ============================
double Spreadsheet::evaluate(const string& expr) {
    string s = expr.substr(1);
    char op = 0;
    size_t pos = string::npos;
    if ((pos = s.find('+')) != string::npos) op = '+';
    else if ((pos = s.find('-')) != string::npos) op = '-';
    else if ((pos = s.find('*')) != string::npos) op = '*';
    else if ((pos = s.find('/')) != string::npos) op = '/';
    else return 0;
    string left = s.substr(0,pos);
    string right = s.substr(pos+1);
    double a = resolveToken(left);
    double b = resolveToken(right);
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return (b != 0) ? a / b : 0;
    }

    return 0;
}


// ============================
// SUMA
// ============================

double Spreadsheet::evaluateSUMA(const string& expr) {

    // quitar "=SUMA(" y ")"
    string inside = expr.substr(6, expr.size() - 7);

    auto range = parseRange(inside);

    int r1 = range.first.first;
    int c1 = range.first.second;
    int r2 = range.second.first;
    int c2 = range.second.second;

    return matrix.sumRange(r1,c1,r2,c2);
}

double Spreadsheet::evaluatePROMEDIO(const string& expr) {

    // quitar "=PROMEDIO(" y ")"
    string inside = expr.substr(10, expr.size() - 11);

    auto range = parseRange(inside);

    int r1 = range.first.first;
    int c1 = range.first.second;
    int r2 = range.second.first;
    int c2 = range.second.second;

    double sum = matrix.sumRange(r1,c1,r2,c2);

    // contar cuántos son numéricos
    int count = 0;

    for (int r = r1; r <= r2; r++) {
        for (int c = c1; c <= c2; c++) {
            string val = matrix.get(r,c);
            if (isNumber(val))
                count++;
        }
    }

    if (count == 0) return 0;

    return sum / count;
}
double Spreadsheet::evaluateMAX(const string& expr) {

    string inside = expr.substr(5, expr.size() - 6);

    auto range = parseRange(inside);

    int r1 = range.first.first;
    int c1 = range.first.second;
    int r2 = range.second.first;
    int c2 = range.second.second;

    double maxVal = -1e18;
    bool found = false;

    for (int r = r1; r <= r2; r++) {
        for (int c = c1; c <= c2; c++) {

            string val = matrix.get(r,c);

            if (isNumber(val)) {
                double num = stod(val);
                if (!found || num > maxVal) {
                    maxVal = num;
                    found = true;
                }
            }
        }
    }

    return found ? maxVal : 0;
}double Spreadsheet::evaluateMIN(const string& expr) {

    string inside = expr.substr(5, expr.size() - 6);

    auto range = parseRange(inside);

    int r1 = range.first.first;
    int c1 = range.first.second;
    int r2 = range.second.first;
    int c2 = range.second.second;

    double minVal = 1e18;
    bool found = false;

    for (int r = r1; r <= r2; r++) {
        for (int c = c1; c <= c2; c++) {

            string val = matrix.get(r,c);

            if (isNumber(val)) {
                double num = stod(val);
                if (!found || num < minVal) {
                    minVal = num;
                    found = true;
                }
            }
        }
    }

    return found ? minVal : 0;
}
// ============================
// SET CELL (CLAVE)
// ============================

void Spreadsheet::setCell(int r,int c,const string& val){

    if (!val.empty() && val[0] == '=') {

        double result = 0;

        if (isSUMA(val)) {
            result = evaluateSUMA(val);
        }
        else if (isPROMEDIO(val)) {
            result = evaluatePROMEDIO(val);
        }
        else if (isMAX(val)) {
            result = evaluateMAX(val);
        }
        else if (isMIN(val)) {
            result = evaluateMIN(val);
        }
        else {
            result = evaluate(val);
        }

        matrix.insert(r, c, formatDouble(result));

    } else {
        matrix.insert(r,c,val);
    }
}
// ============================
// GET
// ============================

string Spreadsheet::getCell(int r,int c){
    return matrix.get(r,c);
}


// ============================
// PRINT
// ============================

void Spreadsheet::show(){
    matrix.printGrid();
}