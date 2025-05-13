#pragma once
#include <vector>
#include "incidenceMatrix.h"

class dlxSolver{
    private:
        Root* root; // Pointer to the root header
        IncidenceMatrix* incidence_matrix; // Pointer to the incidence matrix
        std::vector<int> solution; // Vector to store the solution
        

    public:
        dlxSolver(IncidenceMatrix& incidence_matrix)/*: root(incidence_matrix.header), incidence_matrix(&incidence_matrix) {}*/; // Constructor
        ~dlxSolver(); // Destructor

        Column* choose_column(); // Choose the next column to cover
        void cover(Column* col); // Cover the column
        void uncover(Column* col); // Uncover the column
        std::vector<int> solve(); // Solve the Sudoku puzzle

}; 