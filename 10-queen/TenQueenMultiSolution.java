/* Java program to provide multiple solutions to the 10 Queen Problem */

// package be.ucll.aldeiri;

public class TenQueenMultiSolution 
{ 
	// The number of Queens to place on the chess board.
	final int N = 10; 

	int solutionNumber = 0;

	// A utility function to print one solution.
	void printSolution(int board[][]) 
	{ 
		solutionNumber++;
		System.out.println(); 
		System.out.print ("Solution number " + solutionNumber); 
		System.out.println(); 
		for (int i = 0; i < N; i++) { 
			for (int j = 0; j < N; j++) {
				String queen;
				if (board[i][j] == 1) queen = "Q";
				else queen = "-";
				System.out.print (queen + " "); 
			}
			System.out.println(); 
		} 
	} 

	/* A utility function to check if a queen can 
	be placed on board[row][col]. Note that this 
	function is called when "col" queens are already 
	placed in columns from 0 to col -1. So we need 
	to check only left side for attacking queens */
	boolean isSafe(int board[][], int row, int col) 
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

	// A recursive utility function to solve N Queen problem.
	boolean solveNQueenProblemRecursive(int board[][], int col) 
	{ 
		/* base case: If all queens are placed 
		then return true */
		if (col >= N) 
			return true; 

		/* Consider this column and try placing 
		this queen in all rows one by one */
		for (int i = 0; i < N; i++) 
		{ 
			/* Check if the queen can be placed on 
			board[i][col] */
			if (isSafe(board, i, col)) 
			{ 
				/* Place this queen in board[i][col] */
				board[i][col] = 1; 

				/* recur to place rest of the queens */
				if (solveNQueenProblemRecursive(board, col + 1) == true) {
					printSolution(board); 
				}

				/* If placing queen in board[i][col] 
				doesn't lead to a solution then 
				remove queen from board[i][col] */
				board[i][col] = 0; // BACKTRACK 
			} 
		} 

		/* If the queen can not be placed in any row in 
		this colum col, then return false */
		return false; 
	} 

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

		solveNQueenProblemRecursive(board, 0);
	} 

	// Main program to run the N-Queen solver.
	public static void main(String args[]) 
	{ 
		TenQueenMultiSolution Queen = new TenQueenMultiSolution(); 
		Queen.solveNQ(); 
	} 
} 
