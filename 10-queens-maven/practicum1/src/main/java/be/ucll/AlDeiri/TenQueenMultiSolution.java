package be.ucll.AlDeiri;


public class TenQueenMultiSolution
{
    // The number of queens to place on the chess board.
    final int N = 10;

    /* This function solves the N Queen problem using
    Backtracking. It mainly uses solveNQueenProblemRecursive () to
    solve the problem. It returns false if queens
    cannot be placed, otherwise, return true and
    prints placement of queens in the form of 1s.
    Please note that there may be more than one
    solutions, this function prints one of the
    feasible solutions.*/
    void solveNQ()
    {
        int board[][] = {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        };

        solutionResolveOne.solveNQueenProblemRecursive(board, 0, N);
    }

    // Main program to run the N-Queen solver.
    /*
    public static void main(String args[])
    {
        TenQueenMultiSolution Queen = new TenQueenMultiSolution();
        Queen.solveNQ();
    }
    */
}
