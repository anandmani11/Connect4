
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

// N = no one, R = red, Y = yellow, D = Draw
char winner = 'N';

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

Piece currentPiece;

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

int CheckColorStepper(Piece piece, int stepX, int stepY, char color,
                      Board &board, int steps) {
  if (piece.x < 0 || piece.x >= COLUMNS || piece.y < 0 || piece.y >= ROWS) {
    return steps;
  }

  if (board[piece.y][piece.x].color != color) {
    return steps;
  }
  piece.x += stepX;
  piece.y += stepY;
  return CheckColorStepper(piece, stepX, stepY, color, board, steps + 1);
}

bool CheckWinColor(Board &board, Piece piece, char color) {
  if (color != 'R' && color != 'Y') {
    return false;
  }
  if (piece.x < 0 || piece.x >= COLUMNS || piece.y < 0 || piece.y >= ROWS) {
    return false;
  }

  // 1. Horizontal (Left <-> Right)
  int rightCount = CheckColorStepper({piece.x + 1, piece.y, piece.color}, 1, 0,
                                     color, board, 0);
  int leftCount = CheckColorStepper({piece.x - 1, piece.y, piece.color}, -1, 0,
                                    color, board, 0);
  if (rightCount + leftCount + 1 >= 4)
    return true;

  // 2. Vertical (Up <-> Down)
  int upCount = CheckColorStepper({piece.x, piece.y - 1, piece.color}, 0, -1,
                                  color, board, 0);
  int downCount = CheckColorStepper({piece.x, piece.y + 1, piece.color}, 0, 1,
                                    color, board, 0);
  if (upCount + downCount + 1 >= 4)
    return true;

  // 3. Diagonal Up-Right / Down-Left
  int rightUpCount = CheckColorStepper({piece.x + 1, piece.y - 1, piece.color},
                                       1, -1, color, board, 0);
  int leftDownCount = CheckColorStepper({piece.x - 1, piece.y + 1, piece.color},
                                        -1, 1, color, board, 0);
  if (rightUpCount + leftDownCount + 1 >= 4)
    return true;

  // 4. Diagonal Up-Left / Down-Right
  int leftUpCount = CheckColorStepper({piece.x - 1, piece.y - 1, piece.color},
                                      -1, -1, color, board, 0);
  int rightDownCount = CheckColorStepper(
      {piece.x + 1, piece.y + 1, piece.color}, 1, 1, color, board, 0);
  if (leftUpCount + rightDownCount + 1 >= 4)
    return true;

  return false;
}

bool CheckDraw(Board &board) {
  bool won = true;
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      if (board[i][j].color == 'E') {
        won = false;
        break;
      }
    }
  }
  return won;
}

void CheckWin(Board &board, Piece piece) {
  if (piece.color == 'R' && CheckWinColor(board, piece, 'R')) {
    winner = 'R';
  }
  else if (piece.color == 'Y' && CheckWinColor(board, piece, 'Y')) {
    winner = 'Y';
  }
  else if (CheckDraw(board)) {
    winner = 'D';
  }
  else {
    winner = 'N';
  }
}
void UpdateAndDrawAnimation(Board &board, AnimatingPiece &anim, Piece piece) {
  if (!anim.active)
    return;
  anim.currentY += anim.speed * GetFrameTime();

  if (anim.currentY >= anim.targetY) {
    anim.currentY = anim.targetY;
    anim.active = false;
    board[anim.targetRow][anim.col].color = anim.color;
    Piece placedPiece = {anim.col, anim.targetRow, anim.color};
    CheckWin(board, placedPiece);
  }

  Color drawColor = (anim.color == 'R') ? RED : YELLOW;
  DrawCircle(GetColX(anim.col), anim.currentY, PIECE_SIZE, drawColor);
}

void PlacePiece(Board &board, char &turn, AnimatingPiece &anim, int col) {
  if (anim.active)
    return;

  if (col < 0 || col >= COLUMNS)
    return;

  int targetRow = -1;
  for (int r = ROWS - 1; r >= 0; r--) {
    if (board[r][col].color == 'E') {
      targetRow = r;
      break;
    }
  }

  if (targetRow != -1) {
    currentPiece.x = col;
    currentPiece.y = targetRow;
    currentPiece.color = turn;
    anim.active = true;
    anim.col = col;
    anim.targetRow = targetRow;
    anim.currentY = GetRowY(-1);
    anim.targetY = GetRowY(targetRow);
    anim.color = turn;
    turn = (turn == 'R') ? 'Y' : 'R';
  }
}

void PlayerInput(Board &board, char &turn, AnimatingPiece &anim) {
  Coords coords = GetMouseCoords();
  int col = std::get<0>(coords);

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    PlacePiece(board, turn, anim, col);
  }
}
void ResetGame(Board &board) {
  for (int j = 0; j < ROWS; j++) {
    for (int i = 0; i < COLUMNS; i++) {
      pieces[j][i].x = i;
      pieces[j][i].y = j;
      pieces[j][i].color = 'E';
    }
  }
  winner = 'N';
  turn = 'R';
}

void GameOverMouse(Board &board) {
  if (IsKeyPressed(KEY_R)) {
    ResetGame(board);
  }
}

void GameOverTemplate(std::string text, std::string text2, Board &board) {
  int text1Width = MeasureText(text.c_str(), 80);
  int text2Width = MeasureText(text2.c_str(), 40);

  float boxWidth = (text2Width > text1Width ? text2Width : text1Width) + 60.0f;
  float boxHeight = 180.0f;

  Rectangle rec = {(float)(SCREEN_WIDTH / 2) - (boxWidth / 2),
                   (float)(SCREEN_HEIGHT / 2) - (boxHeight / 2), boxWidth,
                   boxHeight};

  float roundness = 0.2f;
  int segments = 16;

  DrawRectangleRounded(rec, roundness, segments, Fade(WHITE, 0.8f));
  DrawText(text.c_str(), (SCREEN_WIDTH / 2) - (text1Width / 2),
           (SCREEN_HEIGHT / 2) - 50, 80, BLACK);
  DrawText(text2.c_str(), (SCREEN_WIDTH / 2) - (text2Width / 2),
           (SCREEN_HEIGHT / 2) + 40, 40, BLACK);

  GameOverMouse(board);
}

void WinText(Board &board) {
  if (winner == 'R') {
    GameOverTemplate("Red Wins", "Press 'R' To Play Again", board);
  }
  else if (winner == 'Y') {
    GameOverTemplate("Yellow Wins", "Press 'R' To Play Again", board);
  }

  else if (winner == 'D') {
    GameOverTemplate("Draw", "Press 'R' To Play Again", board);
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Connect 4");
  SetTargetFPS(60);
  InitAudioDevice();

  ResetGame(pieces);

  // ------------------------------------------

  Image boardImage = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR);
  for (int j = 0; j < ROWS; j++) {
    for (int i = 0; i < COLUMNS; i++) {
      int cX = (i + 1) * BUFFER_SPACE + i * PIECE_SIZE * 2 + PIECE_SIZE;
      int cY = (j + 1) * BUFFER_SPACE + j * PIECE_SIZE * 2 + PIECE_SIZE;

      for (int y = cY - PIECE_SIZE; y <= cY + PIECE_SIZE; y++) {
        for (int x = cX - PIECE_SIZE; x <= cX + PIECE_SIZE; x++) {
          if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
            if ((x - cX) * (x - cX) + (y - cY) * (y - cY) <=
                PIECE_SIZE * PIECE_SIZE) {
              ((Color *)boardImage.data)[y * SCREEN_WIDTH + x] =
                  Color{0, 0, 0, 0};
            }
          }
        }
      }
    }
  }
  Texture2D boardTexture = LoadTextureFromImage(boardImage);
  UnloadImage(boardImage);

  // ------------------------------------------

  while (WindowShouldClose() == false) {
    if (winner == 'N') {
      PlayerInput(pieces, turn, currentAnimation);
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR);
    DrawPieces(pieces);
    UpdateAndDrawAnimation(pieces, currentAnimation, currentPiece);

    if (winner == 'N') {
      DrawMouseHover();
    }
    else {
      WinText(pieces);
    }

    EndDrawing();
  }

  CloseAudioDevice();

  CloseWindow();
  return 0;
}