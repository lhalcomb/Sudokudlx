#include "sudokuHelper.h"
#include "incidenceMatrix.h"
#include <unistd.h>

using Matrix2D = std::vector<std::vector<int>>; // Define a 2D vector type for the incidence matrix

Matrix2D sudokuHelper::convert_to_matrix(std::string &random_quiz, Matrix2D &grid){ // used in load_sudoku
    // Convert the string to a 2D vector
    // Fill the grid with the numbers from the string

    for (int i = 0; i < 81; i++) {
        grid[i / 9][i % 9] = random_quiz[i] - '0'; // Convert char to int
            //2D              <- 1D Flattened
        }

    return grid;
}

void move_cursor(int x, int y) {
    std::cout << "\033[" << y << ";" << x << "H"; // Move the cursor to (x, y)
}
void sudokuHelper::update_cell(int row, int col, int value) {
    int cursor_row = row + 1 + (row / 3);           // 1-based row + extra lines for '---'
    int cursor_col = col * 2 + 1 + (col / 3 * 2);    // each number takes 2 spaces + '|' spacing
    move_cursor(cursor_row, cursor_col); // Move the cursor to the cell position
    std::cout << value; // Print the value in the cell
}

void sudokuHelper::print_ansi_grid(const Matrix2D &grid) {
    std::cout << "\033[2J\033[1;1H"; // Clear the screen and move the cursor to the top left

    for (int row = 0; row < 9; ++row){
        if (row % 3 == 0 && row != 0) std::cout << "---------------------" << std::endl; 

        for (int col = 0; col < 9; ++col){
            if (col % 3 == 0 && col != 0) std::cout << "| "; 
            char display = grid[row][col] == 0 ? '.' : ('0' + grid[row][col]);
            std::cout << display << ' '; 
        }
        std::cout << std::endl; // New line after each row
    }
    std::cout.flush(); // Flush the output buffer
}

//using bitmasking
/*
    The idea is to use bitmasking to keep track of the numbers that have already appeared in each row, column, and sub-matrix.

        Use a single integer for each row, column, and sub-matrix. Each bit in the integer corresponds to a number.
    For each cell, we can use bitwise AND (&) to check if a number’s bit is already set. If it is, 
    that means the number has appeared before and the Sudoku is invalid.
    If the current cell value (i,e, grid[i][j]) is not set in the row, column, 
    or sub-matrix then we have to set the bit for the number using bitwise OR operation (|=) to mark it visited.
    
*/

bool sudokuHelper::isValid(Matrix2D& grid){
    //Vectors to keep track of the numbers seen in cols, rows, & subBoxes
    std::vector<int> row(9), col(9), subBox(9);
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            if (grid[i][j] == 0) continue; // Skip empty cells
            
            int num = grid[i][j];
            //bit pos for current num
            int pos = 1 << (num - 1); // 1 << (num - 1) gives the bit position for the number

            //chec for duplicates in the current row
            if ((row[i] & pos) > 0) return false; // If the bit is already set, return false
            row[i] |= pos; // Set the bit as seen for the number in the row
            //check for duplicates in the current column
            if ((col[j] & pos) > 0) return false; // If the bit is already set, return false
            col[j] |= pos; // Set the bit as seen for the number in the column
            //check for duplicates in the current sub-box
            int sIdx = (i / 3) * 3 + (j / 3); // Calculate the sub-box index
            if ((subBox[sIdx] & pos) > 0) return false; // If the bit is already set, return false
            subBox[sIdx] |= pos; // Set the bit as seen for the number in the sub-box

        }
    }
    return true; // If no duplicates are found, return true
}

Matrix2D sudokuHelper::load_sudoku(const std::string &filename){
    std::ifstream file(filename); // open the file
    std::string line; // string to hold each line

    std::vector<std::string> quizes; // vector to hold the lines
    if (!file.is_open()){ throw std::runtime_error("Could not open file"); } // check if the file is open

    std::getline(file, line); // read the first line

    //find the index of the quizes column
    std::stringstream header_ss(line); // create a stringstream from the line
    std::string col; // string to hold each column
    int quiz_index = -1; // index of the quizzes column
    int col_index = 0; // index of the column
    while (std::getline(header_ss, col, ',')) { // read each column
        if (col == "quizzes") { // check if the column is quizzes
            quiz_index = col_index; // set the index
            break; // break the loop if found
        }
        col_index++; // increment the index
    }
    if (quiz_index == -1) { throw std::runtime_error("Could not find quizzes column"); } // check if the index is found

    // read the rest of the file
    while (std::getline(file, line)){
        std::stringstream ss(line); // create a stringstream from the line
        std::string cell; // string to hold each cell
        int cell_index = 0; // index of the cell
        while (std::getline(ss, cell, ',')) { // read each cell
            if (cell_index == quiz_index) { // check if the cell is quizzes
                quizes.push_back(cell); // add the cell to the vector
            }
            cell_index++; // increment the index
        }
    }
    srand(static_cast<unsigned>(time(0))); // seed the random number generator
    std::string random_quiz = quizes[rand() % quizes.size()]; // get a random quiz
    Matrix2D grid(9, std::vector<int>(9)); // create a 9x9 grid
    Matrix2D sudoku_grid = convert_to_matrix(random_quiz, grid); // convert the quiz to a matrix

    return sudoku_grid; // return the grid
}

Matrix2D sudokuHelper::solved_grid(std::vector<int> solution){
    Matrix2D solved_grid(9, std::vector<int>(9)); // create a 9x9 grid for the solved Sudoku

    for (int idx : solution){
        int row = idx / 81; // Calculate the row num
        int col = (idx % 81) / 9; // Calculate the column num
        int num = idx % 9;
        solved_grid[row][col] = num + 1; // Fill the grid with the number

    }

    return solved_grid; // return the solved grid
}

void sudokuHelper::print_sudoku(const Matrix2D &grid) {
    for (int row = 0; row < 9; ++row){
        if (row % 3 == 0 && row != 0) std::cout << "---------------------" << std::endl; // Print horizontal separator

        for (int col = 0; col < 9; ++col){
            if (col % 3 == 0 && col != 0) std::cout << "| "; // Print vertical separator
            std::cout << grid[row][col] << " "; // Print the cell value
        }
        std::cout << std::endl; // New line after each row
    }

};


