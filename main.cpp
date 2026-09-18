#include <cctype>
#include <cstdio>
#include <raylib.h>

constexpr int board_size = 768;
constexpr int square_size = board_size / 8;

class Board {
private:
    char board[8][8];
    Texture2D textures[12];
    const char* order = "PNBRQKpnbrqk";
    const char* back_row = "rnbqkbnr";

    void load_textures() {
        for (int i = 0; i < 12; i++) {
            char piece = order[i];
            char color = std::isupper(piece) ? 'w' : 'b';

            char path[64];
            std::snprintf(path, sizeof(path), "../assets/%c%c.png", color, std::tolower(piece));

            textures[i] = LoadTexture(path);
            SetTextureFilter(textures[i], TEXTURE_FILTER_BILINEAR);
        }
    }

    int piece_index(char piece) const {
        for (int i = 0; i < 12; i++) {
            if (order[i] == piece)
                return i;
        }
        return -1;
    }

public:
    Board() {
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                board[row][col] = ' ';
            }
        }

        for (int col = 0; col < 8; col++) {
            board[0][col] = back_row[col];
            board[1][col] = 'p';
            board[6][col] = 'P';
            board[7][col] = std::toupper(back_row[col]);
        }

        load_textures();
    }

    ~Board() {
        for (int i = 0; i < 12; i++) {
            UnloadTexture(textures[i]);
        }
    }

    void draw() const {
        Color light = {238, 238, 213, 255};
        Color dark = {124, 149, 93, 255};

        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                Color color = ((row + col) % 2 == 0) ? light : dark;
                DrawRectangle(col * square_size, row * square_size, square_size, square_size,
                              color);

                int i = piece_index(board[row][col]);
                if (i >= 0) {
                    float scale = (float)square_size / textures[i].width;
                    Vector2 pos = {(float)(col * square_size), (float)(row * square_size)};
                    DrawTextureEx(textures[i], pos, 0.0f, scale, WHITE);
                }
            }
        }
    }
};

void run() {
    Board board;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        board.draw();
        EndDrawing();
    }
}

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(board_size, board_size, "alpaca");
    SetTargetFPS(60);

    run();

    CloseWindow();
    return 0;
}