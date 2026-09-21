#pragma once
#include "Board.h"

// Lớp Game điều khiển vòng lặp, giao diện, tương tác người dùng
class Game {
private:
    Board* board;         // Con trỏ vì kích thước chọn sau khi vào game
    bool   isGameOver;
    bool   isWin;

    void selectDifficulty();   // Menu chọn độ khó

public:
    Game();
    ~Game();                   // Destructor giải phóng board

    void run();                // Vòng lặp chính
};