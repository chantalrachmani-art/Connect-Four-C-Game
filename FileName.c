
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define ROWS 6
#define COLS 7
#define EMPTY ' '

#define RED 12
#define YELLOW 14
#define CYAN 11
#define WHITE 15
#define GRAY 8
#define GREEN 10

static HANDLE console;
static int winsX = 0, winsO = 0, draws = 0;

/* Console design */

void color(int c) {
    SetConsoleTextAttribute(console, (WORD)c);
}

void clearScreen(void) {
    system("cls");
}

void line(void) {
    color(CYAN);
    puts("  +-----------------------------------------------+");
    color(WHITE);
}

void heading(const char* text) {
    line();
    color(CYAN);
    printf("  |  ");
    color(WHITE);
    printf("%-43s", text);
    color(CYAN);
    puts("|");
    line();
}

int readNumber(int min, int max) {
    char input[100], extra;
    int value;

    for (;;) {
        color(YELLOW);
        printf("  > ");
        color(WHITE);

        if (!fgets(input, sizeof input, stdin))
            exit(0);

        if (sscanf(input, " %d %c", &value, &extra) == 1
            && value >= min && value <= max)
            return value;

        color(RED);
        printf("  Please enter a number from %d to %d.\n",
            min, max);
    }
}

void pauseGame(void) {
    char buffer[8];

    color(GRAY);
    puts("\n  Press ENTER to continue...");
    color(WHITE);
    fgets(buffer, sizeof buffer, stdin);
}

/* Board functions */

void initBoard(char board[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            board[r][c] = EMPTY;
}

void printBoard(const char board[ROWS][COLS]) {
    color(CYAN);
    puts("\n       1   2   3   4   5   6   7");
    puts("     +---+---+---+---+---+---+---+");

    for (int r = 0; r < ROWS; ++r) {
        color(CYAN);
        printf("     |");

        for (int c = 0; c < COLS; ++c) {
            color(WHITE);
            printf(" ");

            if (board[r][c] == 'X')
                color(RED);
            else if (board[r][c] == 'O')
                color(YELLOW);
            else
                color(GRAY);

            printf("%c",
                board[r][c] == EMPTY ? '.' : board[r][c]);

            color(CYAN);
            printf(" |");
        }

        puts("");
        puts("     +---+---+---+---+---+---+---+");
    }

    color(WHITE);
}

int drop(char board[ROWS][COLS], int col, char piece) {
    if (col < 0 || col >= COLS ||
        board[0][col] != EMPTY)
        return 0;

    for (int r = ROWS - 1; r >= 0; --r) {
        if (board[r][col] == EMPTY) {
            board[r][col] = piece;
            return 1;
        }
    }

    return 0;
}

/* Win detection */

int win(const char board[ROWS][COLS], char p) {
    const int dr[4] = { 0, 1, 1, 1 };
    const int dc[4] = { 1, 0, 1, -1 };

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (board[r][c] != p)
                continue;

            for (int d = 0; d < 4; ++d) {
                int ok = 1;

                for (int k = 1; k < 4; ++k) {
                    int rr = r + k * dr[d];
                    int cc = c + k * dc[d];

                    if (rr < 0 || rr >= ROWS ||
                        cc < 0 || cc >= COLS ||
                        board[rr][cc] != p) {
                        ok = 0;
                        break;
                    }
                }

                if (ok)
                    return 1;
            }
        }
    }

    return 0;
}

int full(const char board[ROWS][COLS]) {
    for (int c = 0; c < COLS; ++c) {
        if (board[0][c] == EMPTY)
            return 0;
    }

    return 1;
}

/* Computer AI */

int winningColumn(char board[ROWS][COLS], char piece) {
    for (int c = 0; c < COLS; ++c) {
        if (board[0][c] != EMPTY)
            continue;

        char temp[ROWS][COLS];

        for (int r = 0; r < ROWS; ++r)
            for (int j = 0; j < COLS; ++j)
                temp[r][j] = board[r][j];

        drop(temp, c, piece);

        if (win(temp, piece))
            return c;
    }

    return -1;
}

int computerMove(char board[ROWS][COLS], int difficulty) {
    int col;

    if (difficulty >= 2) {
        col = winningColumn(board, 'O');

        if (col >= 0)
            return col;

        col = winningColumn(board, 'X');

        if (col >= 0)
            return col;
    }

    if (difficulty == 3) {
        int order[COLS] = { 3, 2, 4, 1, 5, 0, 6 };

        for (int i = 0; i < COLS; ++i) {
            if (board[0][order[i]] == EMPTY)
                return order[i];
        }
    }

    int available[COLS], count = 0;

    for (int c = 0; c < COLS; ++c) {
        if (board[0][c] == EMPTY)
            available[count++] = c;
    }

    return count ? available[rand() % count] : -1;
}

/* Statistics */

void stats(void) {
    clearScreen();
    heading("GAME STATISTICS");

    color(RED);
    printf("\n  Player X wins : %d\n", winsX);

    color(YELLOW);
    printf("  Player O wins : %d\n", winsO);

    color(WHITE);
    printf("  Draws         : %d\n", draws);

    pauseGame();
}

/* Instructions */

void instructions(void) {
    clearScreen();
    heading("HOW TO PLAY");

    puts("\n  Connect four pieces in a row to win.");
    puts("  You can win horizontally, vertically, or diagonally.");
    puts("  Enter a column number (1-7) to drop your piece.");
    puts("  X plays first. O is the second player or computer.");
    puts("  Difficulty: Easy=random, Medium=win/block,");
    puts("              Hard=win/block + center preference.");

    pauseGame();
}

/* Main game */

void play(int againstComputer, int difficulty) {
    char board[ROWS][COLS];
    int playAgain = 1;

    while (playAgain) {
        initBoard(board);
        int turn = 0;

        for (;;) {
            char piece = (turn % 2 == 0) ? 'X' : 'O';

            clearScreen();
            heading(againstComputer ?
                "PLAYER VS COMPUTER" : "PLAYER VS PLAYER");

            color(piece == 'X' ? RED : YELLOW);
            printf("\n  Current turn: Player %c%s\n",
                piece,
                againstComputer && piece == 'O' ?
                " (Computer)" : "");

            printBoard(board);

            int col;

            if (againstComputer && piece == 'O') {
                color(YELLOW);
                puts("\n  Computer is thinking...");
                Sleep(500);
                col = computerMove(board, difficulty);
            }
            else {
                puts("\n  Choose a column (1-7):");
                col = readNumber(1, 7) - 1;
            }

            if (!drop(board, col, piece)) {
                color(RED);
                puts("  That column is full. Try another.");
                Sleep(900);
                continue;
            }

            if (win(board, piece)) {
                clearScreen();
                heading("FOUR IN A ROW - WINNER!");
                printBoard(board);

                color(GREEN);
                printf("\n  *** PLAYER %c WINS! ***\n", piece);

                if (piece == 'X')
                    ++winsX;
                else
                    ++winsO;

                break;
            }

            if (full(board)) {
                clearScreen();
                heading("FOUR IN A ROW - DRAW");
                printBoard(board);

                color(YELLOW);
                puts("\n  It's a draw!");
                ++draws;
                break;
            }

            ++turn;
        }

        color(WHITE);
        puts("\n  [1] Play again    [2] Main menu");

        playAgain = (readNumber(1, 2) == 1);
    }
}

/* Main menu */

int main(void) {
    console = GetStdHandle(STD_OUTPUT_HANDLE);

    srand((unsigned)time(NULL));

    SetConsoleTitleA("Connect Four | C Console Game");

    for (;;) {
        clearScreen();
        heading("CONNECT FOUR - COLOR EDITION");

        color(WHITE);

        puts("\n  [1] Player vs Player");
        puts("  [2] Player vs Computer");
        puts("  [3] View Statistics");
        puts("  [4] How to Play");
        puts("  [5] Exit");

        puts("\n  Select an option (1-5):");

        int choice = readNumber(1, 5);

        if (choice == 1) {
            play(0, 0);
        }
        else if (choice == 2) {
            clearScreen();
            heading("CHOOSE AI DIFFICULTY");

            puts("\n  [1] Easy   - Random moves");
            puts("  [2] Medium - Win or block immediate threats");
            puts("  [3] Hard   - Win/block + prefer center");

            puts("\n  Select difficulty (1-3):");

            int difficulty = readNumber(1, 3);
            play(1, difficulty);
        }
        else if (choice == 3) {
            stats();
        }
        else if (choice == 4) {
            instructions();
        }
        else {
            color(CYAN);
            puts("\n  Thanks for playing!");
            color(WHITE);
            break;
        }
    }

    return 0;
}
