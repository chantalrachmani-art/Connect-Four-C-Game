#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// Color Definitions
#define RED_COLOR "\033[1;31m"
#define YELLOW_COLOR "\033[1;33m"
#define CYAN_COLOR "\033[1;36m"
#define RESET_COLOR "\033[0m"

#define EMPTY ' '
#define PLAYER1 'X'
#define COMPUTER 'O'
#define PLAYER2 'O'

// Global Statistics
int p1Wins = 0, p2Wins = 0, gameDraws = 0;

// Function Prototypes
void initBoard(char board[6][7]);
void printBoard(char board[6][7]);
int dropDisc(char board[6][7], int col, char piece);
int checkWin(char board[6][7], char piece);
int isBoardFull(char board[6][7]);
int getComputerMove(char board[6][7], int difficulty);
void runGame(int vsComputer, int difficulty);
void enableColors();
void showStats();

int main() {
    enableColors();
    srand((unsigned int)time(NULL));
    int choice = 0;

    while (choice != 4) {
        printf(CYAN_COLOR "\n======= FOUR IN A ROW - PRO EDITION =======\n" RESET_COLOR);
        printf("1. Player vs Player\n2. Player vs Computer\n3. View Statistics\n4. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }

        if (choice == 1) runGame(0, 0);
        else if (choice == 2) {
            int diff;
            printf("\nDifficulty: 1-Easy, 2-Medium, 3-Hard: ");
            if (scanf("%d", &diff) != 1) { while (getchar() != '\n'); continue; }
            runGame(1, diff);
        }
        else if (choice == 3) showStats();
    }
    return 0;
}

void enableColors() {
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
}

void initBoard(char board[6][7]) {
    for (int i = 0; i < 6; i++) for (int j = 0; j < 7; j++) board[i][j] = EMPTY;
}

void printBoard(char board[6][7]) {
    printf("\n    1   2   3   4   5   6   7\n  -----------------------------\n");
    for (int i = 0; i < 6; i++) {
        printf("  |");
        for (int j = 0; j < 7; j++) {
            if (board[i][j] == PLAYER1) printf(" %s%c%s |", RED_COLOR, board[i][j], RESET_COLOR);
            else if (board[i][j] == COMPUTER) printf(" %s%c%s |", YELLOW_COLOR, board[i][j], RESET_COLOR);
            else printf(" %c |", board[i][j]);
        }
        printf("\n  -----------------------------\n");
    }
}

int dropDisc(char board[6][7], int col, char piece) {
    if (col < 0 || col >= 7 || board[0][col] != EMPTY) return 0;
    for (int i = 5; i >= 0; i--) if (board[i][col] == EMPTY) { board[i][col] = piece; return 1; }
    return 0;
}

int checkWin(char board[6][7], char piece) {
    for (int i = 0; i < 6; i++) for (int j = 0; j < 4; j++)
        if (board[i][j] == piece && board[i][j + 1] == piece && board[i][j + 2] == piece && board[i][j + 3] == piece) return 1;
    for (int i = 0; i < 3; i++) for (int j = 0; j < 7; j++)
        if (board[i][j] == piece && board[i + 1][j] == piece && board[i + 2][j] == piece && board[i + 3][j] == piece) return 1;
    for (int i = 0; i < 3; i++) for (int j = 0; j < 4; j++)
        if (board[i][j] == piece && board[i + 1][j + 1] == piece && board[i + 2][j + 2] == piece && board[i + 3][j + 3] == piece) return 1;
    for (int i = 3; i < 6; i++) for (int j = 0; j < 4; j++)
        if (board[i][j] == piece && board[i - 1][j + 1] == piece && board[i - 2][j + 2] == piece && board[i - 3][j + 3] == piece) return 1;
    return 0;
}

int isBoardFull(char board[6][7]) {
    for (int j = 0; j < 7; j++) if (board[0][j] == EMPTY) return 0;
    return 1;
}

int getComputerMove(char board[6][7], int difficulty) {
    if (difficulty >= 2) {
        char pieces[2] = { COMPUTER, PLAYER1 };
        for (int p = 0; p < 2; p++) {
            for (int j = 0; j < 7; j++) {
                for (int i = 5; i >= 0; i--) {
                    if (board[i][j] == EMPTY) {
                        board[i][j] = pieces[p];
                        if (checkWin(board, pieces[p])) { board[i][j] = EMPTY; return j; }
                        board[i][j] = EMPTY; break;
                    }
                }
            }
        }
    }
    if (difficulty == 3) {
        int order[] = { 3, 2, 4, 1, 5, 0, 6 };
        for (int k = 0; k < 7; k++) if (board[0][order[k]] == EMPTY) return order[k];
    }
    int c; do { c = rand() % 7; } while (board[0][c] != EMPTY);
    return c;
}

void showStats() {
    printf(CYAN_COLOR "\n--- STATISTICS ---\nP1 Wins: %d | P2 Wins: %d | Draws: %d\n" RESET_COLOR, p1Wins, p2Wins, gameDraws);
}

void runGame(int vsComputer, int difficulty) {
    char board[6][7];
    initBoard(board);
    int turn = 0, win = 0;
    while (!win && !isBoardFull(board)) {
        printBoard(board);
        char currentPiece = (turn % 2 == 0) ? PLAYER1 : (vsComputer ? COMPUTER : PLAYER2);
        int col;
        if (turn % 2 != 0 && vsComputer) { printf("Computer thinking...\n"); Sleep(800); col = getComputerMove(board, difficulty); }
        else { printf("Player %c, column (1-7): ", currentPiece); if (scanf("%d", &col) != 1) { while (getchar() != '\n'); continue; } col--; }
        if (dropDisc(board, col, currentPiece)) {
            if (checkWin(board, currentPiece)) {
                printBoard(board);
                printf("\n*** PLAYER %c WINS! ***\n", currentPiece);
                if (currentPiece == PLAYER1) p1Wins++; else p2Wins++;
                win = 1;
            }
            turn++;
        }
    }
    if (!win) { printBoard(board); printf("\nDRAW!\n"); gameDraws++; }
}