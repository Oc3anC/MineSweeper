#pragma once
#include <vector>
#include "Cell.h"

// Lớp Board quản lý ma trận các Cell và toàn bộ logic game
class Board {
private:
    int rows, cols, mineCount;
    std::vector<std::vector<Cell>> grid;
    int  revealedCount;  // Số ô đã mở
    int  flagCount;      // Số cờ đã cắm
    bool firstClick;     // Đánh dấu lần click đầu tiên

    // Các hàm hỗ trợ nội bộ (private helper)
    bool isValid(int r, int c) const;
    void placeMines(int safeR, int safeC);
    int  countFlagsAround(int r, int c) const;

public:
    Board(int r, int c, int m);

    // Hành động của người chơi
    void reveal(int r, int c);       // Click trái
    void toggleFlag(int r, int c);   // Click phải
    void chord(int r, int c);        // Click cả 2

    // Truy vấn trạng thái
    bool isWin()  const;
    bool isLose() const;

    // Hiển thị
    void print(bool showAll = false) const;
};