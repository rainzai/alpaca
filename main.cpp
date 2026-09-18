#include <cctype>
#include <raylib.h>

const int SIZE = 768;
const int SQUARE = SIZE / 8;

class Board {
private:
    char board[8][8];

public:
    Board() {
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                board[row][col] = ' ';
            }
        }

        const char* back = "rnbqkbnr";
        for (int col = 0; col < 8; col++) {
            board[0][col] = back[col];
            board[1][col] = 'p';
            board[6][col] = 'P';
            board[7][col] = std::toupper(back[col]);
        }
    }

    void draw() const {
        Color light = {238, 238, 213, 255};
        Color dark = {124, 149, 93, 255};

        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                Color color = ((row + col) % 2 == 0) ? light : dark;
                DrawRectangle(col * SQUARE, row * SQUARE, SQUARE, SQUARE, color);
            }
        }
    }
};

int main() {
    Board board;

    InitWindow(SIZE, SIZE, "alpaca");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        board.draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}