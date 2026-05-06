#include <iostream>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "Spreadsheet.h"

using namespace std;

// ============================
// Auxiliares (funciones de apoyo)
// ============================

// Detecta si un string es un número válido
bool isNumber(const string& s) {
    if (s.empty()) return false;
    char* endptr = nullptr;
    strtod(s.c_str(), &endptr);
    return (*endptr == '\0');
}

// Detecta si un string empieza con "A" o "B", porque es un A7 o B9
bool isCellRef(const string& s) {
    return !s.empty() && isalpha(s[0]);
}

// Convierte una referencia "A1" en coordenadas (fila, columna)
pair<int,int> parseCell(const string& ref) {
    // toupper convierte a Mayuscula
    int col = toupper(ref[0]) - 'A' + 1; // ASCII 'A' - 'A'(65) +1 = col 1, B(66), col 2
    int row = stoi(ref.substr(1)); // toma desde el íncide 1 hasta el final, ejm A1, toma 1
    
    // retorna la col y row, como par
    return {row, col};
}


// Convierte un rango tipo "A1:C3" en dos pares de coordenadas
pair<pair<int,int>, pair<int,int>> parseRange(const string& inside) {
    // busca el carácter ':' que separa las dos referencias
    size_t colon = inside.find(':');

    // toma la parte izquierda (ej. "A1") desde el inicio hasta ':'
    string left = inside.substr(0, colon);

    // toma la parte derecha (ej. "C3") desde después de ':' hasta el final
    string right = inside.substr(colon + 1);

    // convierte cada referencia en coordenadas (fila, columna)
    auto [r1,c1] = parseCell(left);   // ej. "A1" -> (1,1)
    auto [r2,c2] = parseCell(right);  // ej. "C3" -> (3,3)

    // retorna un par de pares: coordenadas iniciales y finales
    // ((fila1,col1), (fila2,col2))
    return {{r1,c1}, {r2,c2}};
}


// Formatea un número double en string bonito (entero o con 2 decimales)
string formatDouble(double val) {
    if (val == (int)val)
        return to_string((int)val);
    ostringstream oss;
    oss << fixed << setprecision(2) << val;
    return oss.str();
}

// Hermanos que usan string::rfind() para buscar una subcadena de igual a..

// Detecta si la cadena es una fórmula SUMA
bool isSUMA(const string& s) {
    return s.rfind("=SUMA(", 0) == 0; // el segundo param 0 es para decir que enmpiza a buscas desde 0 index
}
// Detectores de otras funciones de hoja de cálculo, usadas mas abajo en funciones
bool isPROMEDIO(const string& s) { return s.rfind("=PROMEDIO(", 0) == 0; }
bool isMAX(const string& s) { return s.rfind("=MAX(", 0) == 0; }
bool isMIN(const string& s) { return s.rfind("=MIN(", 0) == 0; }



// ============================
// ============================
//      CLASE Spreadsheet
// ============================
// ============================

// ==================================================================
// Constructor: inicializa la matriz dispersa con filas y columnas
// ==================================================================
Spreadsheet::Spreadsheet(int rows,int cols)
    : matrix(rows,cols) {}

// Devuelve referencia a la matriz interna, permite acceder al motor SparseMatrix.cpp
SparseMatrix& Spreadsheet::getMatrix() {
    return matrix;
}

// ============================
// Resolver token   (usa SparseMatrix.cpp)  
// ============================
// Un token es A6, B9, J7.
// Convierte un token (ej: "A1" o "5") en un valor numérico
double Spreadsheet::resolveToken(const string& token) {
    // si empieza con una letra A B C o así, isCellRef ve eso
    if (isCellRef(token)) {
        auto [r,c] = parseCell(token); // parseCell convierte A1 a [1,1]
        // usa get para obtener el valor de la celda, ejm, en A5 esta un 98.9, lo almacena en val
        string val = matrix.get(r,c);
        // este if comprueba si es num o no, si es por ejm "hello", return 0
        if (!isNumber(val))
            return 0;
        return stod(val);
    }
    // mas sencillo, si es un A1+6, ese 6 que entro aca como token, de frente lo convierte a int y devuelve
    if (isNumber(token))
        return stod(token);
    return 0;
}

// ===============================================================
// Evaluación de expresiones simples (clave, despuies de setCell)
// ===============================================================

// Maneja operaciones tipo "=A1+B1" o "=5*2"
double Spreadsheet::evaluate(const string& expr) {
    string s = expr.substr(1); // quitar '='

    // paso de "=A1+B3" a A1+B3
    char op = 0;
    // inicializo la variabe pos, como no encontrada
    size_t pos = string::npos;
    // find devuelve la posicion en el string
    if ((pos = s.find('+')) != string::npos) op = '+';
    else if ((pos = s.find('-')) != string::npos) op = '-';
    else if ((pos = s.find('*')) != string::npos) op = '*';
    else if ((pos = s.find('/')) != string::npos) op = '/';
    // si no encontro operador o es incorrecto como % por ejm, retorna 0
    else return 0;

    // divido en 2 substrings
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
    // si no entoncto 
    return 0;
}


// ============================
// Funciones de hoja de cálculo
// ============================

// SUMA de un rango
double Spreadsheet::evaluateSUMA(const string& expr) {
    string inside = expr.substr(6, expr.size() - 7); // quitar "=SUMA(" y ")"
    // convierte a (col1, row1, col2, row2)
    auto range = parseRange(inside);

    // llama a la logica de Matrix.cpp
    return matrix.sumRange(range.first.first, range.first.second,
                           range.second.first, range.second.second);
}

// PROMEDIO de un rango
double Spreadsheet::evaluatePROMEDIO(const string& expr) {
    string inside = expr.substr(10, expr.size() - 11); // quitar "=PROMEDIO(" y ")"
    // convierte a (col1, row1, col2, row2)
    auto range = parseRange(inside); // -> par de pares <<x,y><x,y>>
    int r1 = range.first.first, c1 = range.first.second;
    int r2 = range.second.first, c2 = range.second.second;

    // llama a la logica de Matrix.cpp y devuelve la suma
    double sum = matrix.sumRange(r1,c1,r2,c2);
    // Pero aca necesito saber esa suma pero entre la cantidad de celdad no vacias, por eso ese for de abajo
    int count = 0;
    for (int r = r1; r <= r2; r++) {
        for (int c = c1; c <= c2; c++) {
            string val = matrix.get(r,c);
            if (isNumber(val)) count++;
        }
    }
    if (count == 0) return 0;

    // finalmente retorno el prom
    return sum / count;
}


// Máximo de un rango
double Spreadsheet::evaluateMAX(const string& expr) {
    // Quitar el texto "=MAX(" al inicio y ")" al final
    string inside = expr.substr(5, expr.size() - 6);

    // Convertir el rango en coordenadas (fila/col inicial y final)
    auto range = parseRange(inside);
    int r1 = range.first.first, c1 = range.first.second;   // fila y col inicial
    int r2 = range.second.first, c2 = range.second.second; // fila y col final

    // Inicializar el máximo con un valor muy pequeño para asegurar que cualquiera lo supere incialmente
    double maxVal = -1e18;
    bool found = false; // bandera para saber si encontré algún número

    // Recorrer todas las celdas del rango
    for (int r = r1; r <= r2; r++) {
        for (int c = c1; c <= c2; c++) {
            string val = matrix.get(r,c); // leer contenido de la celda
            if (isNumber(val)) {          // si es numérico
                double num = stod(val);   // convertir a double
                // actualizar máximo si es el primero o mayor que el actual
                if (!found || num > maxVal) {
                    maxVal = num;
                    found = true;
                }
            }
        }
    }
    // Si encontré al menos un número, retorno el máximo; si no, retorno 0
    return found ? maxVal : 0;


// Mínimo de un rango
double Spreadsheet::evaluateMIN(const string& expr) {
    // Quitar el texto "=MIN(" al inicio y ")" al final
    string inside = expr.substr(5, expr.size() - 6);

    // Convertir el rango en coordenadas (fila/col inicial y final)
    auto range = parseRange(inside);
    int r1 = range.first.first, c1 = range.first.second;   // fila y col inicial
    int r2 = range.second.first, c2 = range.second.second; // fila y col final

    // Inicializar el máximo con un valor muy grande para asegurar que cualquiera sea mas pequeño incialmente
    double minVal = 1e18;
    bool found = false; // bandera para saber si encontré algún número

    // Recorrer todas las celdas del rango
    for (int r = r1; r <= r2; r++) {
        for (int c = c1; c <= c2; c++) {
            string val = matrix.get(r,c); // leer contenido de la celda
            if (isNumber(val)) {          // si es numérico
                double num = stod(val);   // convertir a double
                // actualizar mínimo si es el primero o menor que el actual
                if (!found || num < minVal) {
                    minVal = num;
                    found = true;
                }
            }
        }
    }
    // Si encontré al menos un número, retorno el mínimo; si no, retorno 0
    return

// ============================
// SET CELL (clave)
// ============================

// Decide si el valor es fórmula o dato directo y lo guarda en la matriz
// Una de dos, o es = algo o es solo string (42, 4.9 , "hello")
void Spreadsheet::setCell(int r,int c,const string& val){
    if (!val.empty() && val[0] == '=') {
        
        // como si es una formula por el = al inicio, uso simples if para ver cual es
        double result = 0;
        if (isSUMA(val)) result = evaluateSUMA(val);
        else if (isPROMEDIO(val)) result = evaluatePROMEDIO(val);
        else if (isMAX(val)) result = evaluateMAX(val);
        else if (isMIN(val)) result = evaluateMIN(val);
        else result = evaluate(val);
        matrix.insert(r, c, formatDouble(result));
    } else {
        matrix.insert(r,c,val);
    }
}




// ============================
// GET
// ============================
// Devuelve el contenido de una celda
string Spreadsheet::getCell(int r,int c){
    return matrix.get(r,c);
}

// ============================
// PRINT
// ============================
// Muestra la hoja en consola (para depuración)
void Spreadsheet::show(){
    matrix.printGrid();
}
