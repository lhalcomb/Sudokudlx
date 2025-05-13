#include "dlxSolver.h"


/*The solver for dlx*/

dlxSolver::dlxSolver(IncidenceMatrix &incidence_matrix)
{
    this->root = incidence_matrix.header; // Set the root header
    this->solution = std::vector<int>(81, 0); // Initialize the solution vector
    this->incidence_matrix = &incidence_matrix; // Set the incidence matrix
}

dlxSolver::~dlxSolver()
{
    // Destructor to clean up the memory
    // No dynamic memory allocation in this class, so nothing to delete
}

Column *dlxSolver::choose_column()
{
    Cell* col = this->root->right; // Start from the first column
    Cell* min_col = col; // Initialize the minimum column
    Column *c = static_cast<Column*>(col); // Cast the cell to a column
    int min_size = c->size; // Initialize the minimum size

    while (col != this->root){ // Loop through the columns
        if (col != nullptr && c->size < min_size){ // Check if the column size is smaller than the minimum size
            min_col = col; // Update the minimum column
            min_size = c->size; // Update the minimum size
        }
        col = col->right; // Move to the next column
    }

    return static_cast<Column*>(min_col); // Return the minimum column

}

void dlxSolver::cover(Column *col) //line 45 'static_cast<Column*>(cell->column)->size--;' is fucking null. why is it null?
{   
    if (col == nullptr) {
        //std::cerr << "Column is null!" << std::endl; // Print an error message if the column is null
        return; // Return if the column is null
    }

    //std::cout << col->left << std::endl;
    col->right->left = col->left; //Link the right column to the left column
    col->left->right = col->right; // Remove the linked column from the list
    Cell* row = col->down; // Start from the first cell in the column
    while (row != col){ // Loop through the cells in the column
        Cell* cell = row->right; // Start from the first cell in the row
        while (cell != row){ // Loop through the cells in the row
            cell->down->up = cell->up; // Link the down cell to the up cell
            cell->up->down = cell->down; // Link the up cell to the down cell
            //std::cout << "Column size: " << cell->column->size << std::endl; // Print the column size
            cell->column->size--; // Decrement the size of the column 
           
            cell = cell->right; // Move to the next cell in the row
        }
        row = row->down; // Move to the next cell in the column
    }
}

void dlxSolver::uncover(Column *col)
{
    Cell* node = col->up; // Move to the column header
        while (node != col){ // Loop through the cells in the column
           Cell* cell = node->left; // Start from the first cell in the row
            while (cell != node){ // Loop through the cells in the row
                cell->column->size++; // Increment the size of the column
                cell->down->up = cell; // Link the down cell to the up cell
                cell->up->down = cell; // Link the up cell to the down cell
                cell = cell->left; // Move to the next cell in the row
            }
            node = node->up; // Move to the next cell in the column
    }
    col->right->left = col; // Link the right column to the left column
    col->left->right = col; // Link the left column to the right column
}
std::vector<int> dlxSolver::solve()
{
    if (this->root->right == this->root){
        //std::cout << "Solution found!" << std::endl; // Print the solution found message
        return this->solution; // Return the solution if the root header is the only column
    }
    Column* column = this->choose_column(); // Choose the next column to cover
    //std::cout << "Covering Column, " << "Column chosen: " << column->name << std::endl; // Print the column name
    this->cover(column); // Cover the column
    Cell* row = column->down; // Start from the first cell in the column
    while (row != column){
        //std::cout << "Row: " << row->name << std::endl; // Print the row name
        this->solution.push_back(std::stoi(row->name.substr(5))); // Add the row index to the solution
        Cell* right_node = row->right; // Start from the first cell in the row
        while (right_node != row){
            this->cover(right_node->column); // Cover the column
            right_node = right_node->right; // Move to the next cell in the row

        }
        std::vector<int> result = this->solve(); // Recursively solve the Sudoku puzzle
        if (!result.empty()){
            return result; // Return the solution if found
        }
        this->solution.pop_back(); // Remove the last row index from the solution
        Cell* left_node = row->left; // Start from the first cell in the row
        while (left_node != row){
            this->uncover(left_node->column); // Uncover the column
            left_node = left_node->left; // Move to the next cell in the row
        }
        row = row->down; // Move to the next cell in the column
    }
    this->uncover(column); // Uncover the column
    //std::cout << "Uncovering Column, " << "Column chosen: " << column->name << std::endl; // Print the column name
    return std::vector<int>(); // Return an empty vector if no solution is found
}

#include "sudokuHelper.h"


int main1(){

    Matrix2D grid = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0 ,6 ,4 ,2 ,3 ,1 ,5 ,8 ,7},
        {2 ,8 ,7 ,4 ,1 ,3 ,5 ,9 ,6},
        {1 ,5 ,9 ,7 ,8 ,6 ,4 ,2 ,3}
    };

    Matrix2D grid2 = sudokuHelper::load_sudoku("sudoku.csv"); // Load the Sudoku grid from a file

    //Validate the Sudoku grid
    //bool valid = sudokuHelper::isValid(grid); // Check if the grid is valid
    bool valid = sudokuHelper::isValid(grid2); // Check if the grid is valid

    if (!valid){
        std::cerr << "Invalid Sudoku grid!" << std::endl; // Print an error message if the grid is invalid
        return 1; // Return an error code
    }
    std::cout << "Valid Sudoku grid!" << std::endl; // Print a success message if the grid is valid


    IncidenceMatrix incidence_matrix(grid); // Create a new incidence matrix object    
    dlxSolver solver(incidence_matrix); // Create a new dlxSolver object
    std::vector<int> solution = solver.solve(); // Solve the Sudoku puzzle

    for (auto& sol: solution){
        std::cout << sol << " "; // Print the solution
    }
    std::cout << std::endl; // Print a new line
    return 0;
}
