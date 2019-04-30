/*dgonzalezvillalo-ereimertburro
Diego Gonzalez Villalobos
Erik Reimert*/

/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  int blockSize; //size of block
  int blockRow, blockColumn; //used to go over block rows and columns
  int row, col; //used to iterate through the blocks in inner loops
  int temporary = 0, d = 0; //d is for diagonal, temporary is a temporary variable
  if (N == 32) {
    blockSize = 8; // choose B = 8 for optimized result.
    //2 outer loops iterates accross blocks / 2 inner loops iterate through each block.
    for (blockColumn = 0; blockColumn < N; blockColumn += 8) {
      for (blockRow = 0; blockRow < N; blockRow +=8) {
        for (row = blockRow; row < blockRow + 8; row++) {
          for (col = blockColumn; col < blockColumn + 8; col++) {
            //when the row and the column are not equal, then we can chage the value in B to the desired in A.
            if (row != col) {
              B[col][row] = A[row][col];
            }
            else {
              // if row == column, it means we touched the diagonal of the square. For thte diagonal blocks, there are conflicts when accessing the same row of A and B. We ued 2 temporary variables, to store position and value for later assign. This way we can avoid having to re-access elements.
              temporary = A[row][col];
              d = row;
            }
          }
          //Each traverse only has 1 element in diagonal. We assign it here, outside of loop.
          if (blockRow == blockColumn) {
            B[d][d]= temporary;
          }
        }
      }

    }
  }
  /* We are using blockSize = 4 here.
  2 levels of loops are used, we assign elements in each row idividually. Causes reduced misses. */
  else if (N == 64) {
    for (blockColumn = 0; blockColumn < N; blockColumn +=4 {
      for (blockRow = 0; blockRow < N; blockRow +=4) {
        for (row =blockRow; row < blockRow + 4; row++) {
          for (col = blockColumn; col < blockColumn + 4; col++) {
            if (row != col) {
              B[col][row] = A[row][col];
            }
            else {
              temporary = A[row][col];
              d = row;
            }
          }
          if (blockRow == blockColumn) {
            B[d][d] = temporary;
          }
        }
      }
    /*
    int columnRun, rowRun,k, a, b, c, d, e, f, g, h; //variables
    //two loops to go through each row and column blocks
    for (columnRun = 0; columnRun < 64; columnRun += 8) {
      for (rowRun = 0; rowRun < 64 ; rowRun += 8 ) {
        //First loop is for a 4 column 8 row of A. Tn the loop we use supporting variables to store all elements of a row. We then try to transpose them with the right positions in B.
        for (k = 0; k <4; k++) {
          a = A[columnRun + k][rowRun+0];
          b = A[columnRun + k][rowRun+1];
          c = A[columnRun + k][rowRun+2];
          d = A[columnRun + k][rowRun+3];
          e = A[columnRun + k][rowRun+4];
          f = A[columnRun + k][rowRun+5];
          g = A[columnRun + k][rowRun+6];
          h = A[columnRun + k][rowRun+7];
          // Not all elements will be transposed correctly. So, elements which can't be transposed correctly will be stored in another location for later use.
          B[rowRun+0][columnRun + k + 0] = a; //correct
          B[rowRun+0][columnRun + k + 4] = f; //incorrect
          B[rowRun+1][columnRun + k + 0] = b; //correct
          B[rowRun+1][columnRun + k + 4] = g; // incorrect
          B[rowRun+2][columnRun + k + 0] = c; //correct
          B[rowRun+2][columnRun + k + 4] = h; //incorrect
          B[rowRun+3][columnRun + k + 0] = d; //correct
          B[rowRun+3][columnRun + k + 4] = e;  //incorrect
        }
        //moving sub-matrix b to sub- matrix c, and moving A->B for sub-matrix b and move matrix d
        // Now that we have dealth with the first 4 col 8 row of A. We know deal with the incorrect assignments.
        a = A[columnRun + 4][rowRun+4];
        b = A[columnRun +5][rowRun+4];
        c = A[columnRun + 6][rowRun+4];
        d = A[columnRun + 7][rowRun+4];
        e = A[columnRun + 4][rowRun+3];
        f = A[columnRun + 5][rowRun+3];
        g = A[columnRun + 6][rowRun+3];
        h = A[columnRun + 7][rowRun+3];

        B[rowRun + 4][columnRun + 0] = B[rowRun + 3][columnRun + 4];
        B[rowRun + 4][columnRun + 4] = a;
        B[rowRun + 3][columnRun + 4]= e;
        B[rowRun + 4][columnRun + 1]= B[rowRun + 3][columnRun + 5];
        B[rowRun + 4][columnRun + 5] = b;
        B[rowRun + 3][columnRun + 5] = f;
        B[rowRun + 4][columnRun + 2] = B[rowRun + 3][columnRun + 6];
        B[rowRun + 4][columnRun + 6] = c;
        B[rowRun + 3][columnRun + 6] = g;
        B[rowRun + 4][columnRun + 3] = B[rowRun + 3][columnRun + 7];
        B[rowRun + 4][columnRun + 7] = d;
        B[rowRun + 3][columnRun + 7] = h;

        //this loop deal with remaining elements.
        for (k = 0;k<3;k++) {
          a = A[columnRun + 4][rowRun + 5 + k];
          b = A[columnRun + 5][rowRun + 5 + k];
          c = A[columnRun + 6][rowRun + 5 + k];
          d = A[columnRun + 7][rowRun + 5 + k];
          e = A[columnRun + 4][rowRun + k];
          f = A[columnRun + 5][rowRun + k];
          e = A[columnRun + 6][rowRun + k];
          e = A[columnRun + 7][rowRun + k];


          B[rowRun + 5 + k][columnRun + 0] = B[rowRun+k][columnRun + 4];
          B[rowRun + 5 + k][columnRun + 4] = a;
          B[rowRun + k][columnRun + 4] = e;
          B[rowRun + 5 + k][columnRun + 1] = B[rowRun + k][columnRun + 5];
          B[rowRun + 5 + k][columnRun + 5] = b;
          B[rowRun + k][columnRun + 5] = f;
          B[rowRun + 5 + k][columnRun + 2] = B[rowRun + k][columnRun + 6];
          B[rowRun + 5 + k][columnRun + 6] = c;
          B[rowRun + k][columnRun + 6] = g;
          B[rowRun + 5 + k][columnRun + 3] =B[rowRun + k][columnRun + 7];
          B[rowRun + 5 + k][columnRun + 7] = d;
          B[rowRun + k][columnRun + 7] = h;

        }
        }
      }
      */
    }
    //Random size. We use blockSie = 16.
    //2 levels of loops are used to iterate over blocks in column major iteration and 2  levels are ued to go through the blocks.
    else {
      blockSize = 16;
      for (blockColumn = 0; blockColumn < M; blockColumn += blockSize) {
        for (blockRow = 0; blockRow < N; blockRow += blockSize) {
          //not all blocks will be square. Have to check for row < N and column < main
          for (row = blockRow; (row < N) && (row < blockRow + blockSize) ; row ++) {
            for ( col = blockColumn; (col < M) && (col < blockColumn + blockSize); col ++) {
              //row and column are not same
              if (row != col) {
                B[col][row] = A[row][col];
              }
              //row and column are the same
              else {
                temporary = A[row][col];
                d = row;
              }
            }
            //Diagonal
            if (blockRow == blockColumn) {
              B[d][d] = temporary;
            }
          }
        }
      }
    }

  }




/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
