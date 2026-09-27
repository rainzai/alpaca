#include <cctype>
#include <cstdio>
#include <raylib.h>

constexpr int board_size = 768;
constexpr int square_size = board_size / 8;

struct Square {
    int row;
    int col;
};

class Board {
private:
    char board[8][8];
    Texture2D textures[12];
    Color light_color = {238, 238, 210, 255};
    Color dark_color = {118, 150, 86, 255};
    Color highlight_color = {255, 255, 51, 127};

    const char* order = "PNBRQKpnbrqk";
    const char* back_row = "rnbqkbnr";

    bool square_selected = false;
    Square selected_square = {-1, -1};
    Square last_move_from = {-1, -1};
    Square last_move_to = {-1, -1};

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

    bool is_highlighted(int row, int col) const {
        if (square_selected && row == selected_square.row && col == selected_square.col)
            return true;
        if (row == last_move_from.row && col == last_move_from.col)
            return true;
        if (row == last_move_to.row && col == last_move_to.col)
            return true;
        return false;
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
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                Color color = ((row + col) % 2 == 0) ? light_color : dark_color;
                DrawRectangle(col * square_size, row * square_size, square_size, square_size,
                              color);

                if (is_highlighted(row, col)) {
                    DrawRectangle(col * square_size, row * square_size, square_size, square_size,
                                  highlight_color);
                }

                int i = piece_index(board[row][col]);
                if (i >= 0) {
                    float scale = (float)square_size / textures[i].width;
                    Vector2 pos = {(float)(col * square_size), (float)(row * square_size)};
                    DrawTextureEx(textures[i], pos, 0.0f, scale, WHITE);
                }
            }
        }
    }

    void click(Vector2 pos) {
        int col = pos.x / square_size;
        int row = pos.y / square_size;
        Square sq = {row, col};

        if (!square_selected) {
            selected_square = sq;
            square_selected = true;
        } else {
            move(sq);
        }
    }

    void move(Square sq) {
        Square from = selected_square;
        Square to = sq;

        board[to.row][to.col] = board[from.row][from.col];
        board[from.row][from.col] = ' ';

        square_selected = false;
        last_move_from = from;
        last_move_to = to;
    }
};

void run() {
    Board board;

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = GetMousePosition();
            board.click(pos);
        }

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