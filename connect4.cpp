
#include <array>
#include <cmath>
#include <iostream>
#include <raylib.h>
#include <tuple>

const int ROWS = 6;
const int COLUMNS = 7;

const int PIECE_SIZE = 40;

const int BUFFER_SPACE = 10;

const int SCREEN_WIDTH =
    PIECE_SIZE * COLUMNS * 2 + (COLUMNS + 1) * BUFFER_SPACE;
const int SCREEN_HEIGHT = PIECE_SIZE * ROWS * 2 + (ROWS + 1) * BUFFER_SPACE;

char redPiece = 'R';
char yellowPiece = 'Y';
char noPiece = 'E'; // empty

Color CLEAR = Color{255, 255, 255, 0};
Color EMPTY = Color{64, 64, 64, 255};
Color BG_COLOR = Color{36, 44, 158, 255};

class Piece {
public:
  int x = 0;
  int y = 0;
  char color = 'E';
};
using Board = std::array<std::array<Piece, COLUMNS>, ROWS>;
Board pieces;

using Coords = std::tuple<int, int>;

Coords GetMouseCoords() {
  int x = GetMouseX();
  int y = GetMouseY();

  int grid_x = x / (BUFFER_SPACE + PIECE_SIZE * 2);
  int grid_y = y / (BUFFER_SPACE + PIECE_SIZE * 2);

  if (grid_x < 0 || grid_x >= COLUMNS || grid_y < 0 || grid_y >= ROWS) {
    return {-1, -1};
  }

  int centerX =
      (grid_x + 1) * BUFFER_SPACE + grid_x * PIECE_SIZE * 2 + PIECE_SIZE;
  int centerY =
      (grid_y + 1) * BUFFER_SPACE + grid_y * PIECE_SIZE * 2 + PIECE_SIZE;

  int dx = x - centerX;
  int dy = y - centerY;
  if ((dx * dx + dy * dy) > (PIECE_SIZE * PIECE_SIZE)) {
    return {-1, -1};
  }

  return {grid_x, grid_y};
}

void DrawTiles(Board board) {
  Color drawColor = EMPTY;
  for (int i = 0; i < COLUMNS; i++) {
    for (int j = 0; j < ROWS; j++) {
      Piece piece = board[j][i];
      if (piece.color == 'R') {
        drawColor = RED;
      }
      else if (piece.color == 'Y') {
        drawColor = YELLOW;
      }
      else {
        drawColor = EMPTY;
      }
      DrawCircle(
          (piece.x + 1) * BUFFER_SPACE + piece.x * PIECE_SIZE * 2 + PIECE_SIZE,
          (piece.y + 1) * BUFFER_SPACE + piece.y * PIECE_SIZE * 2 + PIECE_SIZE,
          PIECE_SIZE, drawColor);
    }
  }
}

void DrawMouseHover() {
  Coords coords = GetMouseCoords();
  if (std::get<0>(coords) >= 0 && std::get<0>(coords) < COLUMNS &&
      std::get<1>(coords) >= 0 && std::get<1>(coords) < ROWS) {
    DrawCircle(std::get<0>(coords) * (PIECE_SIZE * 2 + BUFFER_SPACE) +
                   BUFFER_SPACE + PIECE_SIZE,
               std::get<1>(coords) * (PIECE_SIZE * 2 + BUFFER_SPACE) +
                   BUFFER_SPACE + PIECE_SIZE,
               PIECE_SIZE, Color{255, 255, 255, 100});
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Connect 4");
  SetTargetFPS(60);
  InitAudioDevice();

  for (int j = 0; j < ROWS; j++) {
    for (int i = 0; i < COLUMNS; i++) {
      pieces[j][i].x = i;
      pieces[j][i].y = j;
      pieces[j][i].color = 'E';
    }
  }

  while (WindowShouldClose() == false) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR);

    DrawTiles(pieces);

    DrawMouseHover();

    EndDrawing();
  }

  CloseAudioDevice();

  CloseWindow();
  return 0;
}