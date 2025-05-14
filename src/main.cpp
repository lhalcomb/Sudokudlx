#include "dlxSolver.h"
#include <iostream>
#include <string>
#include <vector>
#include <sudokuHelper.h>
#include <unistd.h>


std::vector<int> solve_sudoku_dlx(Matrix2D& grid){
    //Fix these squigglie lines
    IncidenceMatrix incidence_matrix(grid); // Create a new incidence matrix object
    dlxSolver solver(incidence_matrix); // Create a new dlxSolver object
    std::vector<int> solution = solver.solve(); // Solve the Sudoku puzzle
    return solution; // Return the solution
}


void solve_sudoku(){
    // Example Sudoku grid (0 represents empty cells)
    Matrix2D grid = sudokuHelper::load_sudoku("sudoku.csv"); // Load the Sudoku grid from a file
    sudokuHelper::print_sudoku(grid); // Print the original grid

    std::vector<int> solution = solve_sudoku_dlx(grid); // Solve the Sudoku puzzle


    // Print the solved Sudoku grid
    Matrix2D solved_grid = sudokuHelper::solved_grid(solution); // Get the solved grid
    std::cout << "Solved Sudoku grid:" << std::endl;
    sudokuHelper::print_sudoku(solved_grid); // Print the solved grid
}

void solve_sudoku_ansi(){
    // Example Sudoku grid (0 represents empty cells)
    Matrix2D grid = sudokuHelper::load_sudoku("sudoku.csv"); // Load the Sudoku grid from a file
    sudokuHelper::print_ansi_grid(grid); // Print the original grid

    std::vector<int> solution = solve_sudoku_dlx(grid); // Solve the Sudoku puzzle

    // Print the solved Sudoku grid
    Matrix2D solved_grid = sudokuHelper::solved_grid(solution); // Get the solved grid
    std::cout << "Solved Sudoku grid:" << std::endl;
    for (int i = 0; i < 81; ++i) {
        int row = i / 9;
        int col = i % 9;
        usleep(1000); 
        sudokuHelper::update_cell(row, col, solved_grid[row][col]); // Update the cell in the grid
    }
}

void solve_all_grids() {
    std::ifstream file("sudoku.csv");
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;
    std::getline(file, line); // Read header
    std::stringstream header_ss(line);
    std::string col;
    int quiz_index = -1;
    for (int i = 0; std::getline(header_ss, col, ','); ++i) {
        if (col == "quizzes") {
            quiz_index = i;
            break;
        }
    }
    if (quiz_index == -1) {
        throw std::runtime_error("Could not find quizzes column");
    }

    std::vector<std::string> quizzes;
    quizzes.reserve(1000000); // If you know approx how many quizzes exist

    while (std::getline(file, line)) {
        int current_col = 0;
        size_t start = 0;
        size_t end = line.find(',');

        while (end != std::string::npos && current_col < quiz_index) {
            start = end + 1;
            end = line.find(',', start);
            ++current_col;
        }

        if (current_col == quiz_index) {
            if (end == std::string::npos)
                quizzes.push_back(line.substr(start));
            else
                quizzes.push_back(line.substr(start, end - start));
        }
    }

    Matrix2D grid(9, std::vector<int>(9)); // Declare once
    for (size_t quiz = 0; quiz < quizzes.size(); ++quiz) {
        std::cout << "Quiz " << quiz + 1 << ": " << quizzes[quiz] << std::endl;
        Matrix2D sudoku_grid = sudokuHelper::convert_to_matrix(quizzes[quiz], grid);

        std::cout << "Original Sudoku grid:" << std::endl;
        sudokuHelper::print_sudoku(sudoku_grid);

        std::vector<int> solution = solve_sudoku_dlx(sudoku_grid);

        std::cout << "Solution (last 9 rows): ";
        for (int i = solution.size() - 9; i < solution.size(); ++i) {
            std::cout << solution[i] << " ";
        }
        std::cout << std::endl;

        Matrix2D solved_grid = sudokuHelper::solved_grid(solution);
        sudokuHelper::print_sudoku(solved_grid);
    }
}


int main(){
    
    //solve_all_grids(); // Solve all Sudoku grids
    solve_sudoku(); // Solve a single Sudoku grid
    //solve_sudoku_ansi(); // Solve a single Sudoku grid with ANSI movement

    return EXIT_SUCCESS;
}