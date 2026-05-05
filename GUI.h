#pragma once
#include <QMainWindow>   // Clase base para ventanas principales en Qt
#include <QTableWidget>  // Widget de tabla (similar a Excel)
#include <QLineEdit>     // Barra de texto para entrada de fórmulas/valores
#include <QPushButton>   // Botones de acción
#include <QLabel>        // Etiquetas de texto
#include <QVBoxLayout>   // Layout vertical para organizar widgets
#include <QHBoxLayout>   // Layout horizontal
#include "Spreadsheet.h" // Tu lógica interna de hoja de cálculo

// Clase GUI: representa la ventana principal de la aplicación
class GUI : public QMainWindow {
    Q_OBJECT   // Macro necesaria para habilitar signals/slots en Qt

public:
    // Constructor: inicializa la ventana principal
    GUI(QWidget* parent = nullptr);

private slots:
    // Slots = funciones que responden a eventos (clics, entradas, etc.)
    void onCellClicked(int row, int col);  // Cuando el usuario selecciona una celda
    void onConfirm();                      // Cuando confirma entrada en la barra de texto
    void onDeleteCell();                   // Elimina la celda seleccionada
    void onDeleteRow();                    // Elimina la fila seleccionada
    void onDeleteCol();                    // Elimina la columna seleccionada
    void onDeleteRange();                  // Elimina un rango de celdas

private:
    // ============================
    // Atributos internos
    // ============================

    Spreadsheet sheet;        // Motor interno de la hoja de cálculo (usa SparseMatrix)
    QTableWidget* table;      // Tabla visual donde se muestran las celdas
    QLineEdit* inputBar;      // Barra de entrada para escribir valores o fórmulas
    QLabel* cellLabel;        // Etiqueta que muestra la celda seleccionada (ej: "A1")
    int selectedRow = -1;     // Fila actualmente seleccionada (-1 = ninguna)
    int selectedCol = -1;     // Columna actualmente seleccionada (-1 = ninguna)

    // ============================
    // Funciones auxiliares
    // ============================

    void refreshTable();      // Actualiza la tabla visual con los datos del Spreadsheet
    QString colName(int c);   // Convierte número de columna en letra (ej: 1 → "A")
};
