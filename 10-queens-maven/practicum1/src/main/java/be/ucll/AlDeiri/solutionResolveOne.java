package be.ucll.AlDeiri;

public class solutionResolveOne {

    public static int solutionNumber = 0;

    // A recursive utility function to solve N Queen problem.
    static boolean solveNQueenProblemRecursive(int board[][], int col, int N)
    {
        // base case: If all queens are placed then return true.
        if (col >= N)
            return true;

        /* Consider this column and try placing this queen in all rows one by one */
        for (int i = 0; i < N; i++)
        {
            /* Check if the queen can be placed on board[i][col] */
            if (solutionIsSafe.isSafe(board, i, col, N))
            {
                /* Place this queen in board[i][col] */
                board[i][col] = 1;

                /* recursively place rest of the queens */
                if (solutionResolveOne.solveNQueenProblemRecursive(board, col + 1, N) == true) {
                    solutionNumber++;
                    solutionPrint.print(board, solutionNumber, N);
                }

                // If placing queen in board[i][col] doesn't lead to a solution then remove queen from board[i][col].
                board[i][col] = 0;
            }
        }

        // If the queen can not be placed in any row in this column, then return false.
        return false;
    }

}
