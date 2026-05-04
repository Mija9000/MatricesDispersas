#include "GUI.h"
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>


// Simulamos con 20 filas y 10 columnas

static const int ROWS = 20;
static const int COLS = 10;

GUI::GUI(QWidget* parent)
    : QMainWindow(parent), sheet(ROWS, COLS)
{
    setWindowTitle("Hoja de Cálculo - Matrices Dispersas");
    resize(900, 600);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // Barra superior
    QHBoxLayout* topBar = new QHBoxLayout();
    cellLabel = new QLabel("Celda: --", this);
    cellLabel->setFixedWidth(80);
    inputBar = new QLineEdit(this);
    inputBar->setPlaceholderText("Escribe un valor o fórmula (ej: =A1+B1)");

    QPushButton* btnConfirm  = new QPushButton("✔ Confirmar", this);
    QPushButton* btnDelete   = new QPushButton("✖ Celda", this);
    QPushButton* btnDelRow   = new QPushButton("Eliminar fila", this);
    QPushButton* btnDelCol   = new QPushButton("Eliminar columna", this);
    QPushButton* btnDelRange = new QPushButton("Eliminar rango", this);

    topBar->addWidget(cellLabel);
    topBar->addWidget(inputBar);
    topBar->addWidget(btnConfirm);
    topBar->addWidget(btnDelete);
    topBar->addWidget(btnDelRow);
    topBar->addWidget(btnDelCol);
    topBar->addWidget(btnDelRange);
    mainLayout->addLayout(topBar);

    // Tabla
    table = new QTableWidget(ROWS, COLS, this);
    table->horizontalHeader()->setDefaultSectionSize(80);
    table->verticalHeader()->setDefaultSectionSize(25);

    QStringList colHeaders;
    for (int c = 0; c < COLS; c++)
        colHeaders << colName(c + 1);
    table->setHorizontalHeaderLabels(colHeaders);

    mainLayout->addWidget(table);

    // Conexiones
    connect(table, &QTableWidget::cellClicked, this, &GUI::onCellClicked);
    connect(btnConfirm,  &QPushButton::clicked, this, &GUI::onConfirm);
    connect(inputBar,    &QLineEdit::returnPressed, this, &GUI::onConfirm);
    connect(btnDelete,   &QPushButton::clicked, this, &GUI::onDeleteCell);
    connect(btnDelRow,   &QPushButton::clicked, this, &GUI::onDeleteRow);
    connect(btnDelCol,   &QPushButton::clicked, this, &GUI::onDeleteCol);
    connect(btnDelRange, &QPushButton::clicked, this, &GUI::onDeleteRange);

    refreshTable();
}

QString GUI::colName(int c) {
    return QString(QChar('A' + c - 1));
}

void GUI::refreshTable() {
    table->blockSignals(true);
    for (int r = 1; r <= ROWS; r++) {
        for (int c = 1; c <= COLS; c++) {
            std::string val = sheet.getCell(r, c);
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(val));
            item->setTextAlignment(Qt::AlignCenter);
            if (!val.empty())
                item->setBackground(QColor(220, 240, 255));
            else
                item->setBackground(QColor(255, 255, 255));
            table->setItem(r - 1, c - 1, item);
        }
    }
    table->blockSignals(false);

    // Restaurar selección visual
    if (selectedRow != -1 && selectedCol != -1)
        table->setCurrentCell(selectedRow - 1, selectedCol - 1);
}

void GUI::onCellClicked(int row, int col) {
    selectedRow = row + 1;
    selectedCol = col + 1;
    cellLabel->setText(QString("%1%2").arg(colName(selectedCol)).arg(selectedRow));
    std::string val = sheet.getCell(selectedRow, selectedCol);
    inputBar->setText(QString::fromStdString(val));
    inputBar->setFocus();
}

void GUI::onConfirm() {
    if (selectedRow == -1) {
        QMessageBox::warning(this, "Aviso", "Primero selecciona una celda.");
        return;
    }
    std::string val = inputBar->text().toStdString();
    sheet.setCell(selectedRow, selectedCol, val);
    refreshTable();
}

void GUI::onDeleteCell() {
    if (selectedRow == -1) {
        QMessageBox::warning(this, "Aviso", "Primero selecciona una celda.");
        return;
    }
    sheet.getMatrix().removeCell(selectedRow, selectedCol);
    inputBar->clear();
    refreshTable();
}

void GUI::onDeleteRow() {
    if (selectedRow == -1) {
        QMessageBox::warning(this, "Aviso", "Primero selecciona una celda de la fila a eliminar.");
        return;
    }
    sheet.getMatrix().removeRow(selectedRow);
    inputBar->clear();
    refreshTable();
    QMessageBox::information(this, "Listo",
        QString("Fila %1 eliminada.").arg(selectedRow));
}

void GUI::onDeleteCol() {
    if (selectedCol == -1) {
        QMessageBox::warning(this, "Aviso", "Primero selecciona una celda de la columna a eliminar.");
        return;
    }
    sheet.getMatrix().removeCol(selectedCol);
    inputBar->clear();
    refreshTable();
    QMessageBox::information(this, "Listo",
        QString("Columna %1 eliminada.").arg(colName(selectedCol)));
}

void GUI::onDeleteRange() {
    // Pedir rango al usuario tipo A1:C3
    bool ok;
    QString rango = QInputDialog::getText(this, "Eliminar rango",
        "Ingresa el rango (ej: A1:C3):", QLineEdit::Normal, "", &ok);

    if (!ok || rango.isEmpty()) return;

    // Parsear
    int colon = rango.indexOf(':');
    if (colon == -1) {
        QMessageBox::warning(this, "Error", "Formato inválido. Usa A1:C3");
        return;
    }

    QString left  = rango.left(colon).trimmed();
    QString right = rango.mid(colon + 1).trimmed();

    if (left.isEmpty() || right.isEmpty()) {
        QMessageBox::warning(this, "Error", "Formato inválido.");
        return;
    }

    int c1 = toupper(left[0].toLatin1())  - 'A' + 1;
    int r1 = left.mid(1).toInt();
    int c2 = toupper(right[0].toLatin1()) - 'A' + 1;
    int r2 = right.mid(1).toInt();

    sheet.getMatrix().removeRange(r1, c1, r2, c2);
    inputBar->clear();
    refreshTable();
    QMessageBox::information(this, "Listo",
        QString("Rango %1 eliminado.").arg(rango));
}