#include <cctype>
#include <cmath>
#include <cstdio>
#include <raylib.h>

constexpr int board_size = 768;
constexpr int square_size = board_size / 8;

struct Square {
    int row;
    int col;

    bool operator==(const Square& other) const {
        return row == other.row && col == other.col;
    }
};

enum Side { White, Black };

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

    Side side_to_move = White;

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

    bool is_own_piece(int row, int col) const {
        char piece = board[row][col];
        if (piece == ' ')
            return false;
        Side side = std::isupper(piece) ? White : Black;
        return side == side_to_move;
    }

    bool is_legal_pawn(Square from, Square to) {
        char piece = board[from.row][from.col];
        Side side = std::isupper(piece) ? White : Black;

        int dir = (side == White) ? -1 : 1;
        int start_row = (side == White) ? 6 : 1;
        int dr = (to.row - from.row) * dir;
        int dc = std::abs(to.col - from.col);

        if (dr == 1 && dc == 0) {
            return board[to.row][to.col] == ' ';
        }
        if (dr == 2 && dc == 0 && from.row == start_row) {
            return is_path_clear(from, to) && board[to.row][to.col] == ' ';
        }
        if (dr == 1 && dc == 1) {
            return board[to.row][to.col] != ' ';
        }

        return false;
    }

    bool is_legal_knight(Square from, Square to) {
        int dr = std::abs(to.row - from.row);
        int dc = std::abs(to.col - from.col);

        return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
    }

    bool is_legal_bishop(Square from, Square to) {
        int dr = std::abs(to.row - from.row);
        int dc = std::abs(to.col - from.col);

        if (dr != dc) {
            return false;
        }

        return is_path_clear(from, to);
    }

    bool is_legal_rook(Square from, Square to) {
        int dr = std::abs(to.row - from.row);
        int dc = std::abs(to.col - from.col);

        if (dr == 0 || dc == 0) {
            return is_path_clear(from, to);
        }
        return false;
    }

    bool is_legal_queen(Square from, Square to) {
        return is_legal_bishop(from, to) || is_legal_rook(from, to);
    }

    bool is_legal_king(Square from, Square to) {
        int dr = std::abs(to.row - from.row);
        int dc = std::abs(to.col - from.col);

        return (dr <= 1 && dc <= 1);
    }

    bool is_path_clear(Square from, Square to) {
        int step_r = (to.row > from.row) - (to.row < from.row);
        int step_c = (to.col > from.col) - (to.col < from.col);

        int r = from.row + step_r;
        int c = from.col + step_c;

        while (r != to.row || c != to.col) {
            if (board[r][c] != ' ') {
                return false;
            }
            r += step_r;
            c += step_c;
        }
        return true;
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

        if (is_own_piece(row, col)) {
            selected_square = sq;
            square_selected = true;
        } else if (square_selected && is_legal(selected_square, sq)) {
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

        side_to_move = (side_to_move == White) ? Black : White;
    }

    bool is_legal(Square from, Square to) {
        if (from == to || is_own_piece(to.row, to.col)) {
            return false;
        }

        switch (std::tolower(board[from.row][from.col])) {
        case 'p':
            return is_legal_pawn(from, to);
        case 'n':
            return is_legal_knight(from, to);
        case 'b':
            return is_legal_bishop(from, to);
        case 'r':
            return is_legal_rook(from, to);
        case 'q':
            return is_legal_queen(from, to);
        case 'k':
            return is_legal_king(from, to);
        default:
            return false;
        }
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