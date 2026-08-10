
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

char turn = 'R';

Color CLEAR = Color{255, 255, 255, 0};
Color EMPTY = Color{64, 64, 64, 255};
Color BG_COLOR = Color{36, 44, 158, 255};

struct AnimatingPiece {
  bool active = false;
  int col = 0;
  int targetRow = 0;
  float currentY = 0.0f;
  float targetY = 0.0f;
  float speed = 1600.0f;
  char color = 'E';
};

AnimatingPiece currentAnimation;

float GetRowY(int row) {
  return (row + 1) * BUFFER_SPACE + row * PIECE_SIZE * 2 + PIECE_SIZE;
}

float GetColX(int col) {
  return (col + 1) * BUFFER_SPACE + col * PIECE_SIZE * 2 + PIECE_SIZE;
}

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

  if (grid_x < 0 || grid_x >= COLUMNS) {
    return {-1, -1};
  }

  int grid_y = y / (BUFFER_SPACE + PIECE_SIZE * 2);
  if (grid_y < 0 || grid_y >= ROWS) {
    grid_y = 0;
  }

  return {grid_x, grid_y};
}

void DrawPiece(Piece piece) {
  Color drawColor = EMPTY;
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

void DrawPieces(Board board) {
  for (int i = 0; i < COLUMNS; i++) {
    for (int j = 0; j < ROWS; j++) {
      Piece piece = board[j][i];
      DrawPiece(piece);
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

void UpdateAndDrawAnimation(Board &board, AnimatingPiece &anim) {
  if (!anim.active)
    return;
  anim.currentY += anim.speed * GetFrameTime();

  if (anim.currentY >= anim.targetY) {
    anim.currentY = anim.targetY;
    anim.active = false;
    board[anim.targetRow][anim.col].color = anim.color;
  }
  Color drawColor = (anim.color == 'R') ? RED : YELLOW;
  DrawCircle(GetColX(anim.col), anim.currentY, PIECE_SIZE, drawColor);
}

void PlayerInput(Board &board, char &turn, AnimatingPiece &anim) {
  if (anim.active)
    return;
  Coords coords = GetMouseCoords();
  int col = std::get<0>(coords);

  if (col < 0 || col >= COLUMNS)
    return;

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    int targetRow = -1;
    for (int r = ROWS - 1; r >= 0; r--) {
      if (board[r][col].color == 'E') {
        targetRow = r;
        break;
      }
    }

    if (targetRow != -1) {
      anim.active = true;
      anim.col = col;
      anim.targetRow = targetRow;
      anim.currentY = GetRowY(-1);
      anim.targetY = GetRowY(targetRow);
      anim.color = turn;
      turn = (turn == 'R') ? 'Y' : 'R';
    }
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

    DrawPieces(pieces);

    DrawMouseHover();
    PlayerInput(pieces, turn, currentAnimation);

    if (!currentAnimation.active) {
      DrawMouseHover();
    }

    UpdateAndDrawAnimation(pieces, currentAnimation);

    EndDrawing();
  }

  CloseAudioDevice();

  CloseWindow();
  return 0;
}