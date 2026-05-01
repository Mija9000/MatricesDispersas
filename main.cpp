#include <iostream>
#include "Spreadsheet.h"
#include "SparseMatrix.h"

using namespace std;

int main() {

    cout << "===== INICIO DE PRUEBAS =====\n\n";

    Spreadsheet sheet(5,5);

    // ==========================
    // 1. INSERTAR CELDA
    // ==========================
    cout << "1) Insertar celdas\n";
    sheet.setCell(1,1,"10");  // A1
    sheet.setCell(1,2,"20");  // B1
    sheet.setCell(2,1,"30");  // A2
    sheet.setCell(2,2,"hola"); // B2
    sheet.show();

    // ==========================
    // 2. CONSULTAR CELDA
    // ==========================
    cout << "2) Consultar celda A1 y B2\n";
    cout << "A1: " << sheet.getCell(1,1) << endl;
    cout << "B2: " << sheet.getCell(2,2) << endl;
    cout << "C3 (vacia): " << sheet.getCell(3,3) << endl;

    // ==========================
    // 3. MODIFICAR CELDA
    // ==========================
    cout << "\n3) Modificar celda A1 (10 -> 50)\n";
    sheet.setCell(1,1,"50"); // actualiza
    sheet.show();

    // ==========================
    // 4. ELIMINAR CELDA
    // ==========================
    cout << "4) Eliminar celda B2\n";
    sheet.setCell(2,2,""); // alternativa simple
    sheet.show();

    // ==========================
    // ACCESO DIRECTO A MATRIZ PARA OPERACIONES AVANZADAS
    // ==========================
    cout << "\n===== OPERACIONES AVANZADAS =====\n";
    SparseMatrix& m = sheet.getMatrix();
    // ==========================
    // 5. ELIMINAR FILA
    // ==========================
    cout << "5) Eliminar fila 1\n";
    m.removeRow(1);
    sheet.show();

    // ==========================
    // REINSERTAR DATOS
    // ==========================
    cout << "Reinsertando datos...\n";
    sheet.setCell(1,1,"10");
    sheet.setCell(1,2,"20");
    sheet.setCell(2,1,"30");
    sheet.setCell(3,1,"40");
    sheet.show();

    // ==========================
    // 6. ELIMINAR COLUMNA
    // ==========================
    cout << "6) Eliminar columna 1\n";
    m.removeCol(1);
    sheet.show();

    // ==========================
    // REINSERTAR DATOS
    // ==========================
    cout << "Reinsertando datos...\n";
    sheet.setCell(1,1,"10");
    sheet.setCell(2,1,"20");
    sheet.setCell(3,1,"30");
    sheet.setCell(1,2,"5");
    sheet.setCell(2,2,"15");
    sheet.show();

    // ==========================
    // 7. ELIMINAR RANGO
    // ==========================
    cout << "7) Eliminar rango A1:B2\n";
    m.removeRange(1,1,2,2);
    sheet.show();

    // ==========================
    // REINSERTAR PARA AGREGACIONES
    // ==========================
    cout << "Reinsertando para agregaciones...\n";
    sheet.setCell(1,1,"10");
    sheet.setCell(2,1,"20");
    sheet.setCell(3,1,"30");
    sheet.setCell(1,2,"5");
    sheet.setCell(2,2,"15");
    sheet.show();

    // ==========================
    // 8. SUMA FILA / COLUMNA
    // ==========================
    cout << "8) SUMA fila 1: " << m.sumRow(1) << endl;
    cout << "SUMA columna 1: " << m.sumCol(1) << endl;

    // ==========================
    // 9. SUMA RANGO
    // ==========================
    cout << "9) SUMA rango A1:B3: "
         << m.sumRange(1,1,3,2) << endl;

    // ==========================
    // 10. PROMEDIO (usando fórmula)
    // ==========================
    cout << "\n10) PROMEDIO(A1:A3)\n";
    sheet.setCell(1,3,"=PROMEDIO(A1:A3)");
    sheet.show();

    // ==========================
    // 11. MAX / MIN ()
    // ==========================
    cout << "\nMAX y MIN\n";

    sheet.setCell(1,3,"=MAX(A1:A3)");
    sheet.setCell(2,3,"=MIN(A1:A3)");

    sheet.show();

    // ==========================
    // 12. FORMULAS
    // ==========================
    cout << "\n12) Formulas\n";
    sheet.setCell(2,3,"=A1+A2");
    sheet.setCell(3,3,"=A3*2");
    sheet.show();

    cout << "\n===== FIN DE PRUEBAS =====\n";

    return 0;
}