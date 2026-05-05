#pragma once
#include "SparseMatrix.h"
#include <string>

class Spreadsheet {
private:

    // instancia interna que usa SpreadSheet para funcionar, no duplicado
    SparseMatrix matrix;
    
    // funciones de fórmulas tipo Excel
    double evaluate(const std::string& expr);
    double resolveToken(const std::string& token);
    double evaluateSUMA(const std::string& expr);
    double evaluatePROMEDIO(const std::string& expr);
    double evaluateMAX(const std::string& expr);
    double evaluateMIN(const std::string& expr);

public:
    Spreadsheet(int rows, int cols);
    SparseMatrix& getMatrix();
    void setCell(int r,int c,const std::string& val);
    std::string getCell(int r,int c);

    void show();
};