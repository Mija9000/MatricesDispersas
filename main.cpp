#include "Spreadsheet.h"
#include <iostream>

int main() {
    Spreadsheet sheet(5,5);

    sheet.setCell(1,1,"10");
    sheet.setCell(1,3,"30");
    sheet.setCell(2,2,"hola");

    sheet.show();

    return 0;
}