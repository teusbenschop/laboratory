/* Java program to provide a single solution to the 10 Queen Problem */

// package be.ucll.almafragi;

public class TenQueenSingleSolution 
{
	// The number of queens. 
	final int N = 10; 

	void printSolution(int board[][]) 
	{ 
		for (int i = 0; i < N; i++) { 
			for (int j = 0; j < N; j++) {
				String queen;
				if (board[i][j] == 1) queen = "X";
				else queen = "-";
				System.out.print (queen + " "); 
			}
			System.out.println(); 
		} 
	} 

	// Check whether a queen can be placed on board[row][col].
	// Remark: "col" queens are already placed in columns from 0 to col -1. 
	// So only check left side for attacking queens.
	boolean queenIsSafe(int board[][], int row, int col) 
	{ 
		int i, j; 

		// Check this row on left side.
		for (i = 0; i < col; i++) {
			if (board[row][i] == 1) 
				return false; 
		}

		// Check upper diagonal on left side.
		for (i=row, j=col; i>=0 && j>=0; i--, j--) {
			if (board[i][j] == 1) 
				return false; 
		}

		// Check lower diagonal on left side.
		for (i=row, j=col; j>=0 && i<N; i++, j--) {
			if (board[i][j] == 1) 
				return false; 
		}

		return true; 
	} 

	// Solve the N queen problem through recursive processing.
	boolean solveNQueenProblemRecursive (int board[][], int col) 
	{ 
		// If all queens are placed then bail out.
		if (col >= N) 
			return true; 

		// Consider this column and try placing this queen in all rows one by one.
		for (int i = 0; i < N; i++) 
		{ 
			// Check if the queen can be placed on board[i][col].
			if (queenIsSafe(board, i, col)) 
			{ 
				// Place this queen.
				board[i][col] = 1; 

				// Recursively place all remaining queens.
				if (solveNQueenProblemRecursive(board, col + 1) == true) {
					return true; 
				}

				// If placing queen in board[i][col] doesn't lead to a solution
				// then remove the queen from board[i][col]
				board[i][col] = 0;
			} 
		} 

		/* If the queen can not be placed in any row in 
		this colum col, then return false */
		return false; 
	} 

	// Solve the N Queen problem using backtracking. 
	// This prints only one of the feasible solutions.
	boolean solveNQ() 
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

		if (solveNQueenProblemRecursive(board, 0) == false) 
		{ 
			System.out.print("Solution does not exist\n"); 
			return false; 
		} 

		printSolution(board); 
		return true; 
	} 

	public static void main(String args[]) 
	{ 
		TenQueenSingleSolution Queen = new TenQueenSingleSolution(); 
		Queen.solveNQ(); 
	} 
} 
