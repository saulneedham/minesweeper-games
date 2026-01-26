//C - Procedural

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// CONSTANTS
#define boardLength 9 //Columns
#define boardHeight 9 //Rows
#define noSqaures (boardLength*boardHeight)

// CHARACTER CONSTANTS
char mineChar = 'X';
char flagChar = 'F';
char hiddenChar = '.';

// FUNCTIONS
void floodReveal(int r, int c, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]);
void showBoard(char boardType, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]);
void detonateBomb(int r, int c, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]);
void winCheck(bool *win, bool *game, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]);
void placeMines(int board[boardHeight][boardLength], int numMines);
void calculateSquareValues(int h, int board[boardHeight][boardLength]);
void removeRow(int targetRow, int *h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]);

int main() {
    srand(time(NULL)); 

    int noMines = 10;
    int bombsLeft = 3;
    int removesLeft = 1;

    int activeHeight = boardHeight; 

    int systemBoard[boardHeight][boardLength]; 
    char userBoard[boardHeight][boardLength];

    for (int r = 0; r < boardHeight; r++) {
        for (int c = 0; c < boardLength; c++) {
            userBoard[r][c] = hiddenChar;
            systemBoard[r][c] = 0;
        }
    }

    placeMines(systemBoard, noMines);
    calculateSquareValues(activeHeight, systemBoard);
    
    //showBoard('s', activeHeight, systemBoard, NULL);
    
    showBoard('u', activeHeight, NULL, userBoard);
    
    bool game = true;
    bool win = false;
    int choice, row, column;
    
    while (game == true) {
        printf("\n1) Uncover square\n");
        printf("2) Flag/Unflag square\n");
        printf("3) Place bomb (Bombs left: %d)\n", bombsLeft);
        printf("4) Remove row (Removes left: %d)\n", removesLeft);
        printf("\nWhat would you like to do? (1,2,3,4) - ");
        
        scanf("%d", &choice);
            
        if ((choice==3 && bombsLeft==0) || (choice==4 && removesLeft==0)){
            printf("\nYou don't have any left! ");
        }
        else { 
            if (choice < 4) {
                printf("\nEnter the row index (1-%d): ", activeHeight);
                scanf("%d", &row);
                row = row-1;
                
                printf("\nEnter the square on that row (1-%d): ", boardLength);
                scanf("%d", &column);
                column = column-1;
                
                if (choice==1){
                    if (userBoard[row][column] == flagChar) {
                        printf("This square is flagged! Can't uncover\n");
                    }
                    else if (userBoard[row][column] != hiddenChar) {
                        printf("Square already uncovered!\n");
                    }
                    else {
                        if (systemBoard[row][column] == -1) {
                            printf("You hit a mine!\n");
                            userBoard[row][column] = mineChar;
                            game = false; 
                        } 
                        else {
                            floodReveal(row, column, activeHeight, systemBoard, userBoard); 
                        }
                    }
                }
                else if (choice==2){
                    if (userBoard[row][column] == hiddenChar) {
                        userBoard[row][column] = flagChar;
                    }
                    else if (userBoard[row][column] == flagChar){
                        userBoard[row][column] = hiddenChar;
                    }
                }
                else if (choice == 3) {
                    detonateBomb(row, column, activeHeight, systemBoard, userBoard);
                    bombsLeft = bombsLeft - 1;
                }
            }
            else if (choice == 4) {
                printf("\nEnter the row index to remove (1-%d): ", activeHeight);
                scanf("%d", &row);
                removeRow(row - 1, &activeHeight, systemBoard, userBoard);
                removesLeft = removesLeft - 1;
            }
        }
        
        showBoard('u', activeHeight, NULL, userBoard);
        winCheck(&win, &game, activeHeight, systemBoard, userBoard);
    }
    
    if (win) printf("All squares uncovered - you win!");
    else printf("Mine hit - you lose!");
    
    return 0; 
}

// --- FUNCTION DEFINITIONS ---

void placeMines(int board[boardHeight][boardLength], int numMines) {
    int minesPlaced = 0;
    while (minesPlaced < numMines) {
        int r = rand() % boardHeight;
        int c = rand() % boardLength;
        if (board[r][c] != -1) { board[r][c] = -1; minesPlaced++; }
    }
}

void winCheck(bool *win, bool *game, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]) {
    *win = true; 
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < boardLength; c++) {
            if (sysBoard[r][c] != -1) {
                if (userBoard[r][c] != (char)(sysBoard[r][c] + '0')) {
                    *win = false;
                    return; 
                }
            }
        }
    }
    if (*win) *game = false; 
}

void calculateSquareValues(int h, int board[boardHeight][boardLength]) {
    for (int r = 0; r < h; r++) { 
        for (int c = 0; c < boardLength; c++) { 
            if (board[r][c] == -1) {
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                    for (int colOffset = -1; colOffset <= 1; colOffset++) {
                        if (rowOffset == 0 && colOffset == 0) continue;
                        int neighborR = r + rowOffset;
                        int neighborC = c + colOffset;
                        if (neighborR >= 0 && neighborR < h && neighborC >= 0 && neighborC < boardLength) {
                            if (board[neighborR][neighborC] != -1) board[neighborR][neighborC] += 1;
                        }
                    }
                }
            }
        }
    }
}

void showBoard(char boardType, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]) {
    printf("\n     ");
    for (int c = 1; c <= boardLength; c++) printf(" %d ", c);
    printf("\n\n");
    for (int r = 0; r < h; r++) { 
        printf("%d    ", r + 1);
        for (int c = 0; c < boardLength; c++) { 
            if (boardType == 's') { 
                if (sysBoard[r][c] == -1) printf(" %c ", mineChar); 
                else printf(" %c ", (char)(sysBoard[r][c] + '0'));
            } else printf(" %c ", userBoard[r][c]);
        }
        printf("\n");
    }
}

void floodReveal(int startRow, int startCol, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]) {
    int stack[noSqaures][2]; 
    int top = 0; 
    stack[top][0] = startRow; stack[top][1] = startCol; top++;
    while (top > 0) {
        top--;
        int row = stack[top][0]; int col = stack[top][1];
        if (userBoard[row][col] != hiddenChar || userBoard[row][col] == flagChar) continue; 
        userBoard[row][col] = (char)(sysBoard[row][col] + '0');
        if (sysBoard[row][col] == 0) {
            for (int ro = -1; ro <= 1; ro++) {
                for (int co = -1; co <= 1; co++) {
                    int nr = row + ro; int nc = col + co;
                    if (nr >= 0 && nr < h && nc >= 0 && nc < boardLength) {
                        if (userBoard[nr][nc] == hiddenChar) {
                            stack[top][0] = nr; stack[top][1] = nc; top++;
                        }
                    }
                }
            }
        }
    }
}

void detonateBomb(int r, int c, int h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]) {
    for (int ro = -1; ro <= 1; ro++) {
        for (int co = -1; co <= 1; co++) {
            int nr = r + ro; int nc = c + co;
            if (nr >= 0 && nr < h && nc >= 0 && nc < boardLength) {
                if (userBoard[nr][nc] == hiddenChar || userBoard[nr][nc] == flagChar) {
                    if (sysBoard[nr][nc] == -1) userBoard[nr][nc] = mineChar;
                    else userBoard[nr][nc] = (char)(sysBoard[nr][nc] + '0');
                }
            }
        }
    }
}

void removeRow(int targetRow, int *h, int sysBoard[boardHeight][boardLength], char userBoard[boardHeight][boardLength]) {
    for (int r = targetRow; r < (*h) - 1; r++) {
        for (int c = 0; c < boardLength; c++) {
            sysBoard[r][c] = sysBoard[r + 1][c];
            userBoard[r][c] = userBoard[r + 1][c];
        }
    }
    (*h)--; // Decrement height address
    
    for (int r = 0; r < *h; r++) {
        for (int c = 0; c < boardLength; c++) {
            if (sysBoard[r][c] != -1) sysBoard[r][c] = 0;
        }
    }
    calculateSquareValues(*h, sysBoard);

    for (int r = 0; r < *h; r++) {
        for (int c = 0; c < boardLength; c++) {
            if (userBoard[r][c] != hiddenChar && userBoard[r][c] != flagChar && userBoard[r][c] != mineChar) {
                userBoard[r][c] = (char)(sysBoard[r][c] + '0');
            }
        }
    }
}
