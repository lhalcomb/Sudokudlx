#pragma once
#include <vector>
#include <iostream>
#include <string>


typedef std::vector<std::vector<int>> Matrix2D; // Define a 2D vector type for the incidence matrix
struct Cell { // Base class for the cells in the incidence matrix, foundation of the doubly linked list
    Cell* left;
    Cell* right;
    Cell* up;
    Cell* down;
    class Column* column = nullptr;
    
    std::string name;
    Cell(std::string name, Column* column = nullptr) 
    :name(name),  column(column) {}

};

struct Column: public Cell{ // Inherits from Cell
    int size; 
    // Column* left;
    // Column* right;

    Column(std::string name = "", int size = 0):
    Cell(name, this), size(size) /*, left(nullptr), right(nullptr)*/ {}
};

struct Root: public Column{
    Root() : Column("Root", 0) {} // Constructor initializes the root column with a name and size
};

class IncidenceMatrix
{
    private: // Root header, Column list, the matrix itself
         
        std::vector<Column*> columns; 
        Matrix2D SudokuGrid; // The original grid
        //incidence matrix here
        Matrix2D sudoku_incidence; // The incidence matrix itself
        Matrix2D generate_incidence_matrix(Matrix2D& grid);

    public: 
        IncidenceMatrix(Matrix2D& grid);
        ~IncidenceMatrix(); // Destructor to clean up the memory
        Root* header;

        void create_columns();
        void connect_rows();
        void create_doubly_linked_list();

        Matrix2D grid(); // getter for the grid
        std::pair<int, int> shape() const; // getter for the shape of the sudoku incidence matrix

        Cell* get_cell_from_cols(int row_index, int col_index);

        void print_matrix();

        std::vector<int> ones_in_row(int row_index);

};