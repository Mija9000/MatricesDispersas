cmake_minimum_required(VERSION 3.20)
project(MatricesDispersas)

set(CMAKE_CXX_STANDARD 17)

add_executable(MatricesDispersas
        main.cpp
        GUI.cpp
        SparseMatrix.cpp
        Spreadsheet.cpp
)