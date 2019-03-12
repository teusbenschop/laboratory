package be.ucll.AlDeiri;

public class solutionIsSafe {
    /* A utility function to check if a queen can
be placed on board[row][col]. Note that this
function is called when "col" queens are already
placed in columns from 0 to col -1. So we need
to check only left side for attacking queens */
    static boolean isSafe(int board[][], int row, int col, int N)
    {
        int i, j;

        // Check this row on left side.
        for (i = 0; i < col; i++)
            if (board[row][i] == 1)
                return false;

        // Check upper diagonal on left side.
        for (i=row, j=col; i>=0 && j>=0; i--, j--)
            if (board[i][j] == 1)
                return false;

        // Check lower diagonal on left side.
        for (i=row, j=col; j>=0 && i<N; i++, j--)
            if (board[i][j] == 1)
                return false;

        return true;
    }

}

