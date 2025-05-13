#include <iostream> 
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime> 

using Matrix2D = std::vector<std::vector<int>>; // Define a 2D vector type for the incidence matrix

class sudokuHelper{
    public: 
        static bool isValid(Matrix2D& grid); // Function to validate the Sudoku grid
        static Matrix2D load_sudoku(const std::string& filename); // Function to load the Sudoku grid from a file
        static void print_sudoku(const Matrix2D& grid); // Function to print the Sudoku grid
        static Matrix2D solved_grid(std::vector<int> solution); // Function to solve the Sudoku grid
        static Matrix2D convert_to_matrix(std::string &random_quiz, Matrix2D &grid); // Function to convert the quiz string to a matrix
}; 

