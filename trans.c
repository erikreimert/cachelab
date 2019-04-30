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
  //2 levels of loops are used, we assign elements in each row idividually. Causes reduced misses.
  else if (N == 64) {

    for (blockColumn = 0; blockColumn < N; blockColumn +=4) {
      for (blockRow = 0; blockRow < N; blockRow +=4) {
        for (row =blockRow; row < blockRow + 4; row++) { //iterate over the rows
          for (col = blockColumn; col < blockColumn + 4; col++) {
            if (row != col) { //if not diagonal
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
    }

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
