#include "incidenceMatrix.h"
#include <iostream>
#include <string>


// Destructor to clean up the memory
IncidenceMatrix::~IncidenceMatrix(){
    //free the cells under each column 
    for (Column* col: columns){
        if (col != nullptr){ // if col is nullptr, skip
            //free the cells under the column header
            Cell* current = col->down; // point to the first cell under the column header
            while (current && current != col){ //deprecated "current && current != col"
                Cell* next = current->down; // point to the next cell
                delete current; // delete the current cell
                current = next; // move to the next cell
            }
            delete col; // Delete the column header
        }
    }
    //free the root header
    delete header;
}

IncidenceMatrix::IncidenceMatrix(Matrix2D& grid) {
    
    this->SudokuGrid = grid; // Store the original grid
    header = new Root(); // Create the root header
    columns = std::vector<Column*>(324, nullptr); // std::vector<Column*> columns(324, nullptr); // Create the column list
    sudoku_incidence = generate_incidence_matrix(grid);
    create_doubly_linked_list();
}

//return grid
Matrix2D IncidenceMatrix::grid(){
    return SudokuGrid;
}

std::pair<int, int> IncidenceMatrix::shape() const{
    
    int rows = sudoku_incidence.size();
    int cols = rows > 0? sudoku_incidence[0].size(): 0;

    return {rows, cols};

}



Matrix2D IncidenceMatrix::generate_incidence_matrix(Matrix2D& grid){
    Matrix2D sudoku_incidence(729, std::vector<int>(324, 0)); // Initialize the incidence matrix with zeros
   
    
    // Fill the incidence matrix with ones based on the Sudoku constraints
    for (int r = 0; r < 9; ++r){
         //Force the grid to be 9x9
        if (grid[r].size() != 9) {
            std::cerr << "Invalid row length at row " << r << std::endl;
            exit(EXIT_FAILURE);
        }
        for (int c = 0; c < 9; ++c){
            for (int num = 0; num < 9; ++num){
                int row_index = num + c * 9 + r * 9 * 9; // Row index in the incidence matrix
                if (grid[r][c] != 0 && grid[r][c] - 1 != num){
                    continue; // Skip if the cell is already filled with a different number
                }
                sudoku_incidence[row_index][81 * 0 + c + r * 9] = 1; // cell constraint
                sudoku_incidence[row_index][81 * 1 + num + r * 9] = 1; // row constraint
                sudoku_incidence[row_index][81 * 2 + num + c * 9] = 1; // column constraint
                int box_index = (r / 3) * 3 + (c / 3);
                sudoku_incidence[row_index][81 * 3 + num + box_index * 9] = 1; // box constraint
            }
        }
    }
    return sudoku_incidence; // Return the incidence matrix
}

void IncidenceMatrix::create_doubly_linked_list(){
    IncidenceMatrix::create_columns();
    
    IncidenceMatrix::connect_rows();
}


void IncidenceMatrix::create_columns(){
    Column* prev_col = this->header; // Start from the root header
    std::pair<int, int> shape = this->shape();
    for (int col_index = 0; col_index < shape.second; ++col_index){
        std::string col_name = "Column " + std::to_string(col_index);
        Column* col = new Column(col_name, 0); // Create a new column header
        col->left = prev_col; // Link to the previous column
        prev_col->right = col; // Link the previous column to the new column
        this->columns[col_index] = col; // Store the column in the vector
        Cell* prev_row = col; // Move to the next column

        for (int cell_index = 0; cell_index < shape.first; ++cell_index){
            if (this->sudoku_incidence[cell_index][col_index] != 1){
                continue; // Skip if the cell is not filled
            }
            Cell* row = new Cell("Row->" + std::to_string(cell_index), col); // Create a new cell
            col->size++; // Increment the size of the column
            row->up = prev_row; // Link to the previous cell
            prev_row->down = row; // Link the previous cell to the new cell
            prev_row = row; // Move to the next cell
        }
        col->up = prev_row; // Link the last cell to the column header
        prev_row->down = col; // Link the column header to the last cell
        prev_col = col; // Move to the next column
    }  
    // Link the last column to the root header
    this->header->left = prev_col; // Link the root header to the last column
    prev_col->right = this->header; // Link the last column to the root header

    //check the circular dependency of the columns
    // if (prev_col->up->down == prev_col && prev_col->down->up == prev_col){
    //     std::cout << "Circular dependency of the columns is correct" << std::endl;
    // } else {
    //     std::cerr << "Circular dependency of the columns is incorrect" << std::endl;
    // }    


}

void IncidenceMatrix::connect_rows(){
    std::pair<int, int> shape = this->shape();
    for (int row_index = 0; row_index < shape.first; ++row_index){
        std::vector<int> ones_in_row = this->ones_in_row(row_index); // Get the column indices of the ones in the row

        if (ones_in_row.size() == 0){
            continue; // Skip if there are no ones in the row
        }
        Cell* first_cell = this->get_cell_from_cols(row_index, ones_in_row[0]); // Get the first cell in the row
        if (first_cell == nullptr){
            std::cerr << "Null cell encountered during row connection.\n";
            continue;
        }
        Cell* prev_cell = first_cell; // Start from the first cell
        for (size_t col = 1; col < ones_in_row.size(); ++col){
            Cell* cell = this->get_cell_from_cols(row_index, ones_in_row[col]); // Get the next cell in the row

            cell->left = prev_cell; // Link to the previous cell
            prev_cell->right = cell; // Link the previous cell to the next cell

            prev_cell = cell; // Move to the next cell

        }
        first_cell->left = prev_cell; // Link the first cell to the last cell
        prev_cell->right = first_cell; // Link the last cell to the first cell
    }
}

Cell* IncidenceMatrix::get_cell_from_cols(int row_index, int col_index){
    if (this->sudoku_incidence[row_index][col_index] == 0){
        return nullptr; // Return nullptr if the cell is not filled
    }
    Cell* col = this->columns[col_index]; // Get the column header
    Cell* cell = col->down; // Start from the first cell in the column
    while (cell != col){ // Loop through the cells in the column
        int cell_row_index = std::stoi(cell->name.substr(5)); // Extract the row index from the cell name
        if (cell_row_index == row_index){ // Check if the cell matches the row index
            return cell; // Return the cell if it matches the row index
        }
        cell = cell->down; // Move to the next cell
    }
    return nullptr; // Return nullptr if the cell is not found
}

std::vector<int> IncidenceMatrix::ones_in_row(int row_index){
    std::vector<int> ones_in_row;
    std::pair<int, int> shape = this->shape();
    for (int c = 0; c < shape.second; ++c){
        if (this->sudoku_incidence[row_index][c] == 1){
            ones_in_row.push_back(c);
        }
    }
    return ones_in_row; // Return the vector of column indices
}

void IncidenceMatrix::print_matrix(){
   std::vector<std::vector<std::string>> cells = {{this->header->name}}; // Initialize the cells with the header name

   Cell* col = this->header->right; // Start from the first column
   while( col != this->header){ // Loop through the columns
       std::vector<std::string> row_cells;
       Cell* row = col->down; // Start from the first cell in the column
       while(row != col){ // Loop through the cells in the column
           if (row == nullptr) {
                std::cerr << "Null row encountered during print.\n";
                continue;
           }
           row_cells.push_back(row->name); // Add the cell name to the row
           row = row->down; // Move to the next cell
       }
       row_cells.push_back("column size->" + std::to_string(static_cast<Column*>(col)->size)); // Add the column size to the row
       col = col->right; // Move to the next column
       cells.push_back(row_cells); // Add the row to the cells   
   }
    // Print the cells
    for (const auto& row : cells) {
         for (const auto& cell : row) {
              std::cout << cell << " ";
         }
         std::cout << std::endl;
    }

}

int main2(){
    // Example Sudoku grid (0 represents empty cells)
    Matrix2D grid = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0 ,6 ,2 ,3 ,4 ,5 ,6 ,7 ,8},
        {9 ,8 ,7 ,6 ,5 ,4 ,3 ,2 ,1},
        {2 ,8 ,7 ,4 ,1 ,3 ,5 ,9 ,6}
    };

    IncidenceMatrix incidence_matrix(grid); // Create a new incidence matrix object
    incidence_matrix.print_matrix(); // Print the incidence matrix

    return EXIT_SUCCESS;
}