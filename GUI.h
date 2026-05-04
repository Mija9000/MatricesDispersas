#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Spreadsheet.h"

class GUI : public QMainWindow {
    Q_OBJECT

public:
    GUI(QWidget* parent = nullptr);

private slots:
    void onCellClicked(int row, int col);
    void onConfirm();
    void onDeleteCell();
    void onDeleteRow();
    void onDeleteCol();
    void onDeleteRange();      // <-- nuevo

private:
    Spreadsheet sheet;
    QTableWidget* table;
    QLineEdit* inputBar;
    QLabel* cellLabel;
    int selectedRow = -1;
    int selectedCol = -1;

    void refreshTable();
    QString colName(int c);
};