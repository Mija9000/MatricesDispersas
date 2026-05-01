#pragma once
#include "SparseMatrix.h"

class Spreadsheet {
private:
    SparseMatrix matrix;

public:
    Spreadsheet(int rows, int cols);

    void setCell(int r,int c,const std::string& val);
    std::string getCell(int r,int c);

    void show();
};