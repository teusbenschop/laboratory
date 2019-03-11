public class solutionPrint {
    // A utility function to print one solution.
    static void print(int board[][], int solutionNumber, int N)
    {
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

}
