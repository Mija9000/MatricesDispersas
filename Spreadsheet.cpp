#include "Spreadsheet.h"

Spreadsheet::Spreadsheet(int rows,int cols)
    : matrix(rows,cols) {}

void Spreadsheet::setCell(int r,int c,const std::string& val){
    matrix.insert(r,c,val);
}

std::string Spreadsheet::getCell(int r,int c){
    return matrix.get(r,c);
}

void Spreadsheet::show(){
    matrix.printGrid();
}